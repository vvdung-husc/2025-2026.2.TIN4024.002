/*
=====================================
THÔNG TIN NHÓM 6
1. Phan Bá Bảo Đăng
2. Phạm Văn Bình Minh
3. Võ Văn	Hoàng	
4. Cao Thọ Hà	Tuyên
=====================================
DỰ ÁN: ESP32_DHT_OLED
MÔ TẢ: Hiển thị nhiệt độ, độ ẩm lên OLED
       và điều khiển LED theo ngưỡng nhiệt độ
=====================================
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

/* ===== CẤU HÌNH OLED ===== */
#define OLED_WIDTH   128
#define OLED_HEIGHT  32
#define OLED_ADDR    0x3C
#define OLED_SDA     21
#define OLED_SCL     22

Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

/* ===== CẤU HÌNH DHT22 ===== */
#define DHT_PIN   17
#define DHT_TYPE  DHT22
DHT dht(DHT_PIN, DHT_TYPE);

/* ===== CẤU HÌNH LED ===== */
#define LED_GREEN   15
#define LED_YELLOW  2
#define LED_RED     4

/* ===== THỜI GIAN ===== */
#define SENSOR_INTERVAL  3000   // ms
#define BLINK_INTERVAL   1000    // ms

/* ===== CẤU TRÚC DỮ LIỆU ===== */
struct SensorData {
  float temperature;
  float humidity;
};

struct TempStatus {
  const char* label;
  uint8_t ledPin;
};

/* ===== BIẾN TOÀN CỤC ===== */
SensorData env;
TempStatus currentStatus;

unsigned long lastSensorRead = 0;
unsigned long lastBlink = 0;
bool ledState = false;
bool dhtOK = false;

/* ===== KHAI BÁO HÀM ===== */
void readDHT();
void analyzeTemperature();
void blinkLED();
void displayOLED();

/* ===== SETUP ===== */
void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed");
    while (true);
  }

  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  dht.begin();
  delay(2000);   // ⭐ BẮT BUỘC: chờ DHT22 sẵn sàng
}

/* ===== LOOP ===== */
void loop() {
  unsigned long now = millis();

  if (now - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = now;
    readDHT();
    analyzeTemperature();
  }

  if (now - lastBlink >= BLINK_INTERVAL) {
    lastBlink = now;
    ledState = !ledState;
    blinkLED();
  }

  displayOLED();
}

/* ===== ĐỌC DHT22 ===== */
void readDHT() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    dhtOK = false;
    Serial.println("DHT22 not ready");
    return;
  }

  env.temperature = t;
  env.humidity = h;
  dhtOK = true;

  Serial.print("Temp: ");
  Serial.print(env.temperature);
  Serial.print(" °C | Humi: ");
  Serial.print(env.humidity);
  Serial.println(" %");
}

/* ===== PHÂN TÍCH NHIỆT ĐỘ ===== */
void analyzeTemperature() {
  if (!dhtOK) return;

  if (env.temperature < 13) {
    currentStatus = {"TOO COLD", LED_GREEN};
  }
  else if (env.temperature < 20) {
    currentStatus = {"COLD", LED_GREEN};
  }
  else if (env.temperature < 25) {
    currentStatus = {"COOL", LED_YELLOW};
  }
  else if (env.temperature < 30) {
    currentStatus = {"WARM", LED_YELLOW};
  }
  else if (env.temperature <= 35) {
    currentStatus = {"HOT", LED_RED};
  }
  else {
    currentStatus = {"TOO HOT", LED_RED};
  }
}

/* ===== LED NHẤP NHÁY ===== */
void blinkLED() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  if (ledState && dhtOK) {
    digitalWrite(currentStatus.ledPin, HIGH);
  }
}

/* ===== HIỂN THỊ OLED ===== */
void displayOLED() {
  oled.clearDisplay();

  oled.setTextSize(1);
  oled.setCursor(10, 0);
  oled.println("ESP32 ENV MONITOR");

  oled.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  if (!dhtOK) {
    oled.setCursor(0, 28);
    oled.println("Reading sensor...");
    oled.display();
    return;
  }

  oled.setCursor(0, 16);
  oled.print("Temp: ");
  oled.print(env.temperature, 1);
  oled.println(" C");

  oled.setCursor(0, 28);
  oled.print("Humi: ");
  oled.print(env.humidity, 1);
  oled.println(" %");

  oled.drawLine(0, 42, 128, 42, SSD1306_WHITE);

  oled.setCursor(0, 50);
  oled.print("Status: ");
  oled.println(currentStatus.label);

  oled.display();
}
