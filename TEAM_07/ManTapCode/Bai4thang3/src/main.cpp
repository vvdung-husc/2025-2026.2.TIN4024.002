#include <Arduino.h>
#define BLYNK_PRINT Serial

/* Điền thông tin Blynk của bạn vào đây */
#define BLYNK_TEMPLATE_ID   "TMPL6MPV1HtAo"
#define BLYNK_TEMPLATE_NAME "ESMART"
#define BLYNK_AUTH_TOKEN    "QwvoQYeTSqLPUCQ6U3vw-OpstiKTwGup"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TM1637Display.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Thông tin WiFi
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// ========== Khai báo Pin TM1637 Display ==========
#define CLK 18
#define DIO 19
TM1637Display display(CLK, DIO);

// ========== Khai báo Pin DHT22 ==========
#define DHTPIN 16
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ========== Biến lưu dữ liệu nhiệt độ, độ ẩm ==========
float temperature = 0;
float humidity = 0;

// ========== Biến cho Countdown (đồng bộ với Blynk) ==========
uint32_t countdownSeconds = 0;
bool countdownRunning = false;
uint32_t lastCountdownUpdate = 0; // millis()

// 0: hiển thị temp/hum, 1: hiển thị đếm ngược

// Khai báo chân Pin cho Relay
#define relayPin1 26
#define relayPin2 25
#define relayPin3 33
#define relayPin4 32

// Kết nối nút bấm trên App Blynk với Relay (inverted for active-low relays)
BLYNK_WRITE(V1) {
  digitalWrite(relayPin1, !param.asInt());
}
BLYNK_WRITE(V2) {
  digitalWrite(relayPin2, !param.asInt());
}
BLYNK_WRITE(V3) {
  digitalWrite(relayPin3, !param.asInt());
}
BLYNK_WRITE(V4) {
  digitalWrite(relayPin4, !param.asInt());
}

void setup() {
  Serial.begin(115200);
  
  // Cấu hình các chân Relay là đầu ra
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);

  // Khởi tạo DHT22
  dht.begin();
  Serial.println("DHT22 initialized!");

  // Khởi tạo TM1637 Display
  display.setBrightness(0x0f); // Max brightness
  display.clear();
  Serial.println("TM1637 Display initialized!");

  // Mặc định tắt hết relay khi mới khởi động (set HIGH for active-low)
  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);
  digitalWrite(relayPin4, HIGH);

  // ==========================================
  // THÊM LỆNH KẾT NỐI WIFI CHI TIẾT TẠI ĐÂY
  // ==========================================
  Serial.println();
  Serial.print("Đang kết nối với WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  // Vòng lặp chờ kết nối WiFi thành công
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 40) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✓ Kết nối WiFi thành công!");
    Serial.print("Địa chỉ IP: ");
    Serial.println(WiFi.localIP());
    
    // ✓ Cấu hình DNS server tĩnh (Google DNS)
    // IPAddress dnsServer(8, 8, 8, 8);
    // WiFi.setDNS(dnsServer);
    // delay(1000);
    
    // Cấu hình và kết nối Blynk sau khi đã có WiFi
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect(); // Timeout 10 giây
  } else {
    Serial.println("✗ Lỗi: Không thể kết nối WiFi!");
  }
}

void updateDisplayCountdown() {
  // Hiển thị mm:ss
  uint32_t seconds = countdownSeconds;
  uint8_t mins = seconds / 60;
  uint8_t secs = seconds % 60;
  uint16_t displayValue = mins * 100 + secs; // mmss

  // 'dots' 0b01000000 tạo 00:00 (colon) trên module có hỗ trợ colon
  display.showNumberDecEx(displayValue, 0b01000000, true, 4, 0);
}

BLYNK_WRITE(V0) {
  // Blynk gửi số giây đếm ngược (ví dụ giá trị 11) -> hiển thị trên 4-digit
  countdownSeconds = param.asInt();
  countdownRunning = (countdownSeconds > 0);
  lastCountdownUpdate = millis();

  updateDisplayCountdown();
}

void loop() {
  Blynk.run();

  // Cập nhật đếm ngược mỗi giây
  if (countdownRunning) {
    uint32_t now = millis();
    if (now - lastCountdownUpdate >= 1000) {
      lastCountdownUpdate = now;
      if (countdownSeconds > 0) {
        countdownSeconds--;
        updateDisplayCountdown();
        if (Blynk.connected()) {
          Blynk.virtualWrite(V0, countdownSeconds);
        }
      }
      if (countdownSeconds == 0) {
        countdownRunning = false;
      }
    }
  }

  // Nếu không đang đếm ngược thì hiển thị nhiệt độ/độ ẩm (cập nhật ~2s)
  static uint32_t lastDhtUpdate = 0;
  if (!countdownRunning && millis() - lastDhtUpdate >= 2000) {
    lastDhtUpdate = millis();

    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("❌ Lỗi đọc DHT22!");
      display.showNumberDec(9999, false); // Hiển thị lỗi
    } else {
      float tempDisplay = temperature;
      int tempInt = (int)(tempDisplay * 10);
      display.showNumberDec(tempInt, true, 4, 0); // Hiển thị NN.N (vd 25.4)

      if (Blynk.connected()) {
        Blynk.virtualWrite(V5, temperature);  // Gửi nhiệt độ
        Blynk.virtualWrite(V6, humidity);     // Gửi độ ẩm
      }

      Serial.print("🌡️  Nhiệt độ: ");
      Serial.print(temperature);
      Serial.print("°C | 💧 Độ ẩm: ");
      Serial.print(humidity);
      Serial.println("%");
    }
  }

  // Logic tự động kết nối lại nếu mất mạng
  if (!Blynk.connected()) {
    Serial.println("Mất kết nối Blynk. Đang thử kết nối lại...");
    Blynk.connect();
    delay(2000);
  }
}