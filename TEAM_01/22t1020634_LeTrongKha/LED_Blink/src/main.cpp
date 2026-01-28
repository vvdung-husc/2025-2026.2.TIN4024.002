#include <Arduino.h>

#define PIN_LED_RED 23

// Non-blocking timer function
bool IsReady(unsigned long &ulTimer, uint32_t millisecond) {
  if (millis() - ulTimer < millisecond) return false;
  ulTimer = millis();
  return true;
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("LED Blink Project - Le Trong Kha");
  Serial.println("Student ID: 22t1020634");
  
  // Set LED pin as output
  pinMode(PIN_LED_RED, OUTPUT);
}

void loop() {
  static unsigned long ulTimer = 0;
  static bool ledStatus = false;

  // Non-blocking LED blink every 1000ms (1 second)
  if (IsReady(ulTimer, 1000)) {
    ledStatus = !ledStatus;
    Serial.printf("LED is %s\n", ledStatus ? "ON" : "OFF");
    digitalWrite(PIN_LED_RED, ledStatus ? HIGH : LOW);
  }
}