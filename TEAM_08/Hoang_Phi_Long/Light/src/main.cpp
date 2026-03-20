#include <Arduino.h>

#define LED_RED    25
#define LED_YELLOW 33
#define LED_GREEN  32

void blinkLedCountdown(int pin, int seconds, const char* name) {
  Serial.print("\nLED [");
  Serial.print(name);
  Serial.print("] ON => ");
  Serial.print(seconds);
  Serial.println(" Seconds");

  // Đếm từ (seconds - 1) xuống 0
  for (int i = seconds - 1; i >= 0; i--) {
    Serial.print("  Times: ");
    Serial.print(i);
    Serial.println(" s");

    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
    delay(500);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  Serial.println("Traffic Light System Started");
  Serial.println("============================");
}

void loop() {
  blinkLedCountdown(LED_RED, 5, "RED");     // 4 → 0
  blinkLedCountdown(LED_YELLOW, 3, "YELLOW"); // 2 → 0
  blinkLedCountdown(LED_GREEN, 7, "GREEN");  // 6 → 0
}