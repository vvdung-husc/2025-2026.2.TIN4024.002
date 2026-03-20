/* Thông tin kết nối Blynk */
#define BLYNK_TEMPLATE_ID "TMPL6IUZiv5tV"
#define BLYNK_TEMPLATE_NAME "Blynk"
#define BLYNK_AUTH_TOKEN "COyH_2Ihbnl2fyQVxSqxa1q49eabID-4"

#include <Arduino.h> // Bắt buộc phải có khi code trên VS Code (PlatformIO)
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TM1637Display.h>

// Thay đổi thông tin WiFi nhà bạn vào đây nhé
char ssid[] = "Ko co mang";
char pass[] = "Brmilahud";

// Định nghĩa chân ESP32 theo mạch của bạn
#define DHTPIN 16          // Chân Data của DHT22
#define DHTTYPE DHT22      // Loại cảm biến

#define CLK_PIN 18         // Chân CLK của TM1637
#define DIO_PIN 19         // Chân DIO của TM1637

#define LED_PIN 21         // Chân đèn LED
#define BUTTON_PIN 23      // Chân nút nhấn

// Khởi tạo các đối tượng
DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK_PIN, DIO_PIN);
BlynkTimer timer;

int uptimeCount = 0;

// Hàm đọc dữ liệu cảm biến và gửi lên Blynk
void sendSensorData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Lỗi không đọc được DHT!");
    return;
  }

  // Gửi dữ liệu nhiệt độ, độ ẩm lên Blynk
  Blynk.virtualWrite(V0, t); 
  Blynk.virtualWrite(V1, h); 
  
  // Tăng biến đếm thời gian và gửi lên Blynk
  uptimeCount++;
  Blynk.virtualWrite(V2, uptimeCount); 

  // Hiển thị số giây lên màn hình LED 7 đoạn TM1637
  display.showNumberDec(uptimeCount, false);
}

// Hàm nhận lệnh từ công tắc trên Blynk (Chân V3)
BLYNK_WRITE(V3) {
  int switchState = param.asInt(); 
  
  if (switchState == 1) {
    digitalWrite(LED_PIN, HIGH); 
    Serial.println("Bật đèn & Chế độ đếm ngược");
  } else {
    digitalWrite(LED_PIN, LOW);  
    Serial.println("Tắt đèn");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Cài đặt chân vào/ra (I/O)
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Khởi tạo màn hình TM1637
  display.setBrightness(0x0f); 
  display.clear();

  // Khởi tạo DHT22 và kết nối Blynk
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Đặt lịch trình: gọi hàm sendSensorData mỗi 2 giây
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  // Duy trì hoạt động của Blynk và Timer
  Blynk.run();
  timer.run();
}