/*
THÔNG TIN NHÓM 4
1. Nguyễn Quốc Bảo
*/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define LED_GREEN 25
#define LED_YELLOW 26
#define LED_RED 27

void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  dht.begin();

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  String status = "";

  if (t < 13) {
    status = "TOO COLD";
    digitalWrite(LED_GREEN, HIGH);
  } else if (t < 20) {
    status = "COLD";
    digitalWrite(LED_GREEN, HIGH);
  } else if (t < 25) {
    status = "COOL";
    digitalWrite(LED_YELLOW, HIGH);
  } else if (t < 30) {
    status = "WARM";
    digitalWrite(LED_YELLOW, HIGH);
  } else if (t < 35) {
    status = "HOT";
    digitalWrite(LED_RED, HIGH);
  } else {
    status = "TOO HOT";
    digitalWrite(LED_RED, HIGH);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.print(status);

  display.setCursor(0, 20);
  display.print("Temp: ");
  display.print(t);
  display.print(" C");

  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.print(h);
  display.print(" %");

  display.display();
  delay(2000);
}
