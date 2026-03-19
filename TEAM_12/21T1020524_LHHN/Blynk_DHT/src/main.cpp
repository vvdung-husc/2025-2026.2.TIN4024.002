// --- ĐIỀN THÔNG TIN BLYNK CỦA BẠN VÀO 3 DÒNG DƯỚI ĐÂY ---
#define BLYNK_TEMPLATE_ID "TMPL6iDMP5B2e"
#define BLYNK_TEMPLATE_NAME "BlynkDHTLeHuuHoangNam"
#define BLYNK_AUTH_TOKEN "gB1HspUqI4lniDgBLM7dnpaq8i307fBV"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TM1637Display.h>
#include <DHT.h>

// Thông tin WiFi của Wokwi (Không thay đổi)
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// --- Cấu hình chân ---
#define DHTPIN 15          // Chân nối DHT22
#define DHTTYPE DHT22      // Loại cảm biến DHT22
DHT dht(DHTPIN, DHTTYPE);

#define CLK 2              // Chân màn hình TM1637
#define DIO 3
TM1637Display display(CLK, DIO);

#define LED_PIN 5          // Đèn LED xanh
#define BTN_PIN 4          // Nút nhấn

// Các biến toàn cục
BlynkTimer timer;
bool systemOn = true;      // Trạng thái bật/tắt hiển thị
int uptimeCounter = 0;     // Biến đếm thời gian

// Nhận lệnh từ nút nhấn trên App Blynk (Kênh V3)
BLYNK_WRITE(V3) {
  systemOn = param.asInt();
  digitalWrite(LED_PIN, systemOn ? HIGH : LOW);
  if (!systemOn) {
    display.clear(); // Xóa màn hình nếu tắt
  }
}

// Hàm đọc và gửi dữ liệu Nhiệt độ, Độ ẩm lên Blynk
void sendSensorData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Kiểm tra nếu lỗi đọc cảm biến thì bỏ qua
  if (isnan(h) || isnan(t)) return;

  // Gửi lên Blynk: V0 là Nhiệt độ, V1 là Độ ẩm
  Blynk.virtualWrite(V0, t); 
  Blynk.virtualWrite(V1, h); 
}

// Hàm đếm thời gian và cập nhật màn hình
void updateUptime() {
  if (systemOn) {
    uptimeCounter++;
    display.showNumberDec(uptimeCounter); // Hiện trên TM1637
    Blynk.virtualWrite(V2, uptimeCounter); // Gửi thời gian lên web/app (V2)
  }
}

void setup() {
  Serial.begin(115200);
  
  // Cài đặt các chân
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, HIGH);
  
  // Khởi tạo thiết bị
  display.setBrightness(0x0f);
  dht.begin();
  
  // Kết nối WiFi và Blynk
  Serial.println("Đang kết nối Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  // Cài đặt Timer chạy ngầm: 2s gửi nhiệt độ 1 lần, 1s đếm ngược 1 lần
  timer.setInterval(2000L, sendSensorData);
  timer.setInterval(1000L, updateUptime);
}

void loop() {
  Blynk.run(); // Chạy ngầm Blynk
  timer.run(); // Chạy ngầm Timer

  // Xử lý nút nhấn vật lý trên mạch Wokwi
  static bool lastBtnState = HIGH;
  bool btnState = digitalRead(BTN_PIN);
  
  if (btnState == LOW && lastBtnState == HIGH) {
    systemOn = !systemOn; // Đảo trạng thái
    digitalWrite(LED_PIN, systemOn ? HIGH : LOW);
    
    if (!systemOn) {
      display.clear();
    }
    
    // Cập nhật trạng thái ngược lại lên nút bấm trên App Blynk
    Blynk.virtualWrite(V3, systemOn); 
    delay(50); // Chống dội phím
  }
  lastBtnState = btnState;
}