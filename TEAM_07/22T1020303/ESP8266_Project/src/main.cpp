#include <Arduino.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <Wire.h>

// 1. Cấu hình LED trên Board (D4 - chân 2 của ESP8266)
const int LED_PIN = 2; 

// 2. Cấu hình DHT11 (Chân D3)
#define DHTPIN 0  // D3 tương ứng chân GPIO 0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// 3. Cấu hình Cảm biến GAS (Chân A0)
#define GAS_PIN A0

// 4. Cấu hình OLED SH1106 (SDA=D2, SCL=D1)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo LED
  pinMode(LED_PIN, OUTPUT);
  
  // Khởi tạo DHT
  dht.begin();
  
  // Khởi tạo OLED
  u8g2.begin();
}

void loop() {
  // --- Điều khiển LED nhấp nháy ---
  digitalWrite(LED_PIN, LOW); // Bật LED (ESP8266 bật ở mức LOW)
  
  // --- Đọc cảm biến ---
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gasValue = analogRead(GAS_PIN);

  // --- Hiển thị OLED ---
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  
  u8g2.drawStr(0, 15, "--- ESP8266 MONITOR ---");
  
  // Hiển thị Nhiệt độ
  u8g2.setCursor(0, 35);
  u8g2.print("Nhiet do: "); u8g2.print(t); u8g2.print(" C");
  
  // Hiển thị Độ ẩm
  u8g2.setCursor(0, 50);
  u8g2.print("Do am: "); u8g2.print(h); u8g2.print(" %");
  
  // Hiển thị Khí Gas
  u8g2.setCursor(0, 64);
  u8g2.print("Khi GAS: "); u8g2.print(gasValue);

  u8g2.sendBuffer();

  delay(500);
  digitalWrite(LED_PIN, HIGH); // Tắt LED
  delay(500);
}