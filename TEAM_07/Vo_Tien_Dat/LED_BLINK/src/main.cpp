#include <Arduino.h>

#define PIN_LED_RED 23

//Non-blocking
bool IsReady(unsigned long &ulTimer, uint32_t millisecond) {
  if (millis() - ulTimer < millisecond) return false;
  ulTimer = millis();
  return true;
}

void setup() {
  // put your setup code here, to run once:
  printf("WELCOME IOT\n");
  pinMode(PIN_LED_RED, OUTPUT);
}

//Non-Blocking
void loop() {
  static unsigned long ulTimer = 0;
  static bool ledStatus = false;

  if (IsReady(ulTimer, 500)){
    ledStatus = !ledStatus;
    printf("LED IS %s\n", ledStatus ? "ON" : "OFF");
    digitalWrite(PIN_LED_RED, ledStatus ? HIGH : LOW);
  }
}