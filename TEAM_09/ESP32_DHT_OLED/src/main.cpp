/*
THÔNG TIN NHÓM 9
1. Nguyễn Đăng Khôi
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

/* ====== OLED ====== */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* ====== DHT ====== */
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

/* ====== LED ====== */
#define LED_GREEN 15
#define LED_YELLOW 2
#define LED_RED 0

unsigned long lastBlink = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  dht.begin();
  Wire.begin(13, 12); // SDA, SCL

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("OLED failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  float temp = dht.readTemperature();
  float humi = dht.readHumidity();

  if (isnan(temp) || isnan(humi)) {
    Serial.println("DHT error");
    return;
  }

  String status;
  int led = -1;

  /* ====== NHIỆT ĐỘ ====== */
  if (temp < 13) {
    status = "TOO COLD";
    led = LED_GREEN;
  } else if (temp < 20) {
    status = "COLD";
    led = LED_GREEN;
  } else if (temp < 25) {
    status = "COOL";
    led = LED_YELLOW;
  } else if (temp < 30) {
    status = "WARM";
    led = LED_YELLOW;
  } else if (temp < 35) {
    status = "HOT";
    led = LED_RED;
  } else {
    status = "TOO HOT";
    led = LED_RED;
  }

  /* ====== OLED ====== */
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.println(status);

  display.setTextSize(2);
  display.setCursor(0, 14);
  display.print(temp, 2);
  display.print(" ");
  display.write(247);
  display.println("C");

  display.setTextSize(1);
  display.setCursor(0, 38);
  display.println("Humidity:");

  display.setTextSize(2);
  display.setCursor(0, 48);
  display.print(humi, 2);
  display.println(" %");

  display.display();

  /* ====== LED BLINK ====== */
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  if (millis() - lastBlink > 500) {
    lastBlink = millis();
    ledState = !ledState;
  }

  digitalWrite(led, ledState);

  delay(200);
}