#include <Arduino.h>

#define PIN_LED_RED     23
#define PIN_LED_YELLOW  22
#define PIN_LED_GREEN   19

// Non-blocking
bool IsReady(unsigned long &ulTimer, uint32_t millisecond) {
  if (millis() - ulTimer < millisecond) return false;
  ulTimer = millis();
  return true;
}


static unsigned long currentLed = 0;

static unsigned long red = 0;
static unsigned long green = 0;
static unsigned long yellow = 0;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
}

void loop() {

  static unsigned long ulTimer = 0;
  static bool ledStatus = false;


  if (red >= 6) {
    currentLed = 1;
    yellow = 0;
    green = 0;
    red = 0;
  }
  if (green >= 10) {     
    currentLed = 2;
    yellow = 0;
    green = 0;
    red = 0;
  }
  if (yellow >= 4) {   
    currentLed = 0;
    yellow = 0;
    green = 0;
    red = 0;
  }

  switch (currentLed) {

    case 0: 
      if (IsReady(ulTimer, 1000)) {
        ledStatus = !ledStatus;
        digitalWrite(PIN_LED_RED, ledStatus);
        digitalWrite(PIN_LED_GREEN, LOW);
        digitalWrite(PIN_LED_YELLOW, LOW);
        red++;
      }
      break;

    case 1: 
      if (IsReady(ulTimer, 1000)) {
        ledStatus = !ledStatus;
        digitalWrite(PIN_LED_GREEN, ledStatus);
        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_YELLOW, LOW);
        green++;
      }
      break;

    case 2: 
      if (IsReady(ulTimer, 1000)) {
        ledStatus = !ledStatus;
        digitalWrite(PIN_LED_YELLOW, ledStatus);
        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_GREEN, LOW);
        yellow++;
      }
      break;
  }
}
