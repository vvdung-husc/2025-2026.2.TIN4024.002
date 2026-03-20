
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

// void loop() {
//   // put your main code here, to run repeatedly:
//   //static int i = 0;
//   //printf("Loop running ... %d\n",++i);
//   //delay(1000);
//   digitalWrite(PIN_LED_RED, HIGH); // LED ON
//   delay(50000); // Wait 500 ms
//   digitalWrite(PIN_LED_RED, LOW); // LED OFF
//   //delay(500); // Wait 500 ms

// }
