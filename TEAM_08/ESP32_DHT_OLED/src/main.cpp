/*
THÔNG TIN NHÓM 8
1. Lê Phước Phúc - 22T1020700
2. Lê Văn Hoàng Thái - 22T1020417
*/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

#define OLED_SDA 13
#define OLED_SCL 12

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= DHT ==================
#define DHTPIN 16
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ================= LED ==================
#define LED_GREEN 15
#define LED_YELLOW 2
#define LED_RED 4

// ================= TIMER =================
unsigned long lastBlink = 0;
bool ledState = false;

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("❌ OLED không khởi động được");
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  dht.begin();
}

// ================= LED CONTROL =================
void blinkLED(int ledPin) {
  if (millis() - lastBlink >= 500) {
    lastBlink = millis();
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}

void turnOffAllLED() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
}

// ================= LOOP =================
void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ Đọc DHT thất bại");
    delay(2000);
    return;
  }

  String status = "";
  String icon = "";
  int activeLED = -1;

  if (temperature < 13) {
    status = "TOO COLD";
    icon = "❄";
    activeLED = LED_GREEN;
  } else if (temperature < 20) {
    status = "COLD";
    icon = "☔";
    activeLED = LED_GREEN;
  } else if (temperature < 25) {
    status = "COOL";
    icon = "☁";
    activeLED = LED_YELLOW;
  } else if (temperature < 30) {
    status = "WARM";
    icon = "☀";
    activeLED = LED_YELLOW;
  } else if (temperature < 35) {
    status = "HOT";
    icon = "🌞";
    activeLED = LED_RED;
  } else {
    status = "TOO HOT";
    icon = "🔥";
    activeLED = LED_RED;
  }

  turnOffAllLED();
  blinkLED(activeLED);

  // ===== OLED DISPLAY =====
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(temperature, 1);
  display.println(" C");

  display.setCursor(0, 16);
  display.print("Humidity: ");
  display.print(humidity, 1);
  display.println(" %");

  display.setTextSize(2);
  display.setCursor(0, 36);
  display.print(status);

  display.display();

  // ===== SERIAL =====
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" | Humidity: ");
  Serial.print(humidity);
  Serial.print(" | Status: ");
  Serial.println(status);

  delay(100);
}
