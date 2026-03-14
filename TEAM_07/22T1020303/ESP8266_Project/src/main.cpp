#include <Arduino.h>
#include <U8g2lib.h>
#include <DHT.h>

// 1. Cấu hình Màn hình OLED SH1106 (SDA = D2, SCL = D1)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 2. Cấu hình Cảm biến DHT11 (Chân D3)
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// 3. Chân cảm biến Gas MQ2 (Chân A0)
#define GAS_PIN A0

void setup() {
  // Khởi tạo Serial để debug
  Serial.begin(115200);
  
  // Khởi tạo OLED
  u8g2.begin();
  
  // Khởi tạo DHT
  dht.begin();
  
  // Khởi tạo đèn LED trên Board (D4 hoặc D0)
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Nhấp nháy đèn LED (Yêu cầu 2)
  digitalWrite(LED_BUILTIN, LOW);  // Bật (Logic âm trên ESP8266)
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH); // Tắt
  delay(200);

  // Đọc nhiệt độ, độ ẩm (Yêu cầu 3)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Đọc khí GAS (Yêu cầu 4)
  int gasValue = analogRead(GAS_PIN);

  // Hiển thị lên màn hình OLED (Yêu cầu 5)
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr); 
  
  u8g2.setCursor(0, 15);
  u8g2.print("Nhiet do: "); u8g2.print(t); u8g2.print(" C");
  
  u8g2.setCursor(0, 35);
  u8g2.print("Do am: "); u8g2.print(h); u8g2.print(" %");
  
  u8g2.setCursor(0, 55);
  u8g2.print("Khi GAS: "); u8g2.print(gasValue);
  
  u8g2.sendBuffer(); // Gửi dữ liệu ra màn hình

  // In ra Serial để kiểm tra trên máy tính
  Serial.print("Temp: "); Serial.print(t);
  Serial.print(" - Gas: "); Serial.println(gasValue);

  delay(1000); // Đợi 1 giây trước khi lặp lại
}