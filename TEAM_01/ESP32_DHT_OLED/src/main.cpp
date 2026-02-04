/*
THÔNG TIN NHÓM 1
1. Đào Hữu Khánh
2. Lê Quang Duy
3. Trần Thị Như Sang
4. Nguyễn Duy Phong
5. Lê Trọng Kha
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ===== LED =====
#define LED_GREEN  25
#define LED_YELLOW 26
#define LED_RED    27

// ===== DHT22 =====
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ===== OLED =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===== LED blink =====
bool ledState = false;
unsigned long lastBlink = 0;
const int blinkInterval = 500;

void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
}

void loop() {
  float temp = dht.readTemperature();
  float humi = dht.readHumidity();

  if (isnan(temp) || isnan(humi)) return;

  // ===== Xác định trạng thái =====
  String status = "";
  int activeLED = 0; // 1 GREEN, 2 YELLOW, 3 RED

  if (temp < 13) {
    status = "TOO COLD";
    activeLED = 1;
  } else if (temp < 20) {
    status = "COLD";
    activeLED = 1;
  } else if (temp < 25) {
    status = "COOL";
    activeLED = 2;
  } else if (temp < 30) {
    status = "WARM";
    activeLED = 2;
  } else if (temp < 35) {
    status = "HOT";
    activeLED = 3;
  } else {
    status = "TOO HOT";
    activeLED = 3;
  }

  // ===== LED nhấp nháy =====
  if (millis() - lastBlink > blinkInterval) {
    lastBlink = millis();
    ledState = !ledState;

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);

    if (ledState) {
      if (activeLED == 1) digitalWrite(LED_GREEN, HIGH);
      if (activeLED == 2) digitalWrite(LED_YELLOW, HIGH);
      if (activeLED == 3) digitalWrite(LED_RED, HIGH);
    }
  }

  // ===== OLED =====
  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("Temp: ");
  display.print(temp);
  display.println(" C");

  display.print("Humi: ");
  display.print(humi);
  display.println(" %");

  display.println();
  display.print("Status: ");
  display.println(status);

  display.display();
}
