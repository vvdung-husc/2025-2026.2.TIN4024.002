/*
 * ESP32 DevKit V1 Project
 * Tác giả: Nguyễn Duy Phong
 * 
 * Lưu ý: Code tương thích với ESP8266, chỉ khác pin number
 * 
 * Chức năng:
 * - LED nhấp nháy
 * - Đọc DHT22 (nhiệt độ, độ ẩm)
 * - Đọc MQ2 (khí gas - dùng joystick thay thế)
 * - Hiển thị lên OLED SSD1306/SH1106
 */

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>

// ===== Pin Configuration ESP32 =====
#define LED_PIN      2     // LED built-in (GPIO2)
#define DHT_PIN      4     // DHT22 sensor (GPIO4)
#define MQ2_PIN      34    // MQ2 analog sensor (GPIO34 - ADC)
#define OLED_SDA     21    // I2C SDA (GPIO21)
#define OLED_SCL     22    // I2C SCL (GPIO22)

// ===== DHT Configuration =====
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

// ===== OLED Configuration =====
// SSD1306 128x64 I2C (Wokwi hỗ trợ SSD1306, tương thích với SH1106)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// ===== Global Variables =====
float temperature = 0.0;
float humidity = 0.0;
int gasValue = 0;
bool ledState = false;

unsigned long previousMillis = 0;
const long interval = 1000; // 1 giây

void updateOLED(); // Forward declaration

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n========================================");
  Serial.println("  ESP32 DevKit V1 - IoT Project");
  Serial.println("  (Tương đương ESP8266 NodeMCU)");
  Serial.println("  Tác giả: Nguyễn Duy Phong");
  Serial.println("========================================");

  // LED setup
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("✓ LED (GPIO2) initialized");

  // DHT sensor setup
  dht.begin();
  Serial.println("✓ DHT22 (GPIO4) initialized");

  // OLED setup
  Wire.begin(OLED_SDA, OLED_SCL);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "ESP32 DevKit V1");
  u8g2.drawStr(0, 25, "Starting...");
  u8g2.drawStr(0, 40, "Phong 2024");
  u8g2.sendBuffer();
  Serial.println("✓ OLED SSD1306 initialized");

  // MQ2 (Analog)
  pinMode(MQ2_PIN, INPUT);
  Serial.println("✓ MQ2 Sensor (GPIO34) initialized");

  delay(2000);

  Serial.println("\n========================================");
  Serial.println("System Ready!");
  Serial.println("========================================\n");
}

void loop() {
  unsigned long currentMillis = millis();

  // Cập nhật mỗi 1 giây
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // ===== Đọc DHT22 =====
    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();

    if (!isnan(newTemp) && !isnan(newHum)) {
      temperature = newTemp;
      humidity = newHum;
    }

    // ===== Đọc MQ2 (Analog) =====
    gasValue = analogRead(MQ2_PIN);

    // ===== Toggle LED =====
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);

    // ===== In ra Serial Monitor =====
    Serial.println("========================================");
    Serial.print("🌡️  Nhiệt độ: ");
    Serial.print(temperature, 1);
    Serial.println(" °C");
    
    Serial.print("💧 Độ ẩm: ");
    Serial.print(humidity, 1);
    Serial.println(" %");
    
    Serial.print("💨 Gas (MQ2): ");
    Serial.println(gasValue);
    
    Serial.print("💡 LED: ");
    Serial.println(ledState ? "ON" : "OFF");
    
    Serial.print("⏱️  Uptime: ");
    Serial.print(millis() / 1000);
    Serial.println(" s");
    Serial.println("========================================\n");

    // ===== Hiển thị lên OLED =====
    updateOLED();
  }
}

// ===== Hàm cập nhật OLED =====
void updateOLED() {
  u8g2.clearBuffer();

  // Header
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "ESP32 - Phong");
  u8g2.drawLine(0, 12, 128, 12);

  // Nhiệt độ
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 25, "Temp:");
  u8g2.setCursor(45, 25);
  u8g2.print(temperature, 1);
  u8g2.drawStr(85, 25, "C");

  // Độ ẩm
  u8g2.drawStr(0, 38, "Hum:");
  u8g2.setCursor(45, 38);
  u8g2.print(humidity, 1);
  u8g2.drawStr(85, 38, "%");

  // Gas
  u8g2.drawStr(0, 51, "Gas:");
  u8g2.setCursor(45, 51);
  u8g2.print(gasValue);

  // LED Status
  u8g2.drawStr(0, 64, "LED:");
  u8g2.drawStr(45, 64, ledState ? "ON" : "OFF");

  u8g2.sendBuffer();
}