#include <Arduino.h>

#define PIN_LED_RED 25
#define PIN_LED_YELLOW 33
#define PIN_LED_GREEN 32

//Non-blocking
bool IsReady(unsigned long &ulTimer, uint32_t millisecond) {
  if (millis() - ulTimer < millisecond) return false;
  ulTimer = millis();
  return true;
}

void setup() {
  // Cài đặt chế độ Output cho các chân đèn
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
}

void loop() {
  static unsigned long ulTimer = 0;
  static int state = 0;

  switch (state)
  {
  case 0: //Đèn đỏ
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);
    if(IsReady(ulTimer, 5000)){
      state = 1;
      printf("LED [YELLOW] ON => 3 Seconds\n");
    }
    break;
  case 1: //Đèn vàng
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_YELLOW,  HIGH);
    digitalWrite(PIN_LED_GREEN, LOW);
    if(IsReady(ulTimer, 3000)){
      state = 2;
      printf("LED [GREEN] ON => 7 Seconds\n");
    }
    break;
  case 2: //Đèn xanh
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_YELLOW,  LOW);
    digitalWrite(PIN_LED_GREEN, HIGH);
    if(IsReady(ulTimer, 7000)){
      state = 0; // Quay về đèn đỏ (state = 0)
      printf("LED [RED] ON => 5 Seconds\n");
    }
    break;
  }
}