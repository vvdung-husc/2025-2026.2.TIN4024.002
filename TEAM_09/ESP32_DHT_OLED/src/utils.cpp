#include "utils.h"

/* ===== Global Objects ===== */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastBlink = 0;
bool ledState = false;

/* ===== OLED INIT ===== */
void setupOLED() {
  Wire.begin(13, 12); // SDA, SCL

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("OLED failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

/* ===== OLED DISPLAY ===== */
void updateOLED(float temp, float humi, String status) {
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
}

/* ===== LED CONTROL ===== */
void updateLED(float temp) {
  String status;
  int led = -1;

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

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  if (millis() - lastBlink > 500) {
    lastBlink = millis();
    ledState = !ledState;
  }

  digitalWrite(led, ledState);
}
