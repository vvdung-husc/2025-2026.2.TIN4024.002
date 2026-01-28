#include <Arduino.h>

// GPIO
#define PIN_LED_RED     25
#define PIN_LED_YELLOW  33
#define PIN_LED_GREEN   32

// Số giây (số lần nháy)
#define RED_TIME     5
#define GREEN_TIME   7
#define YELLOW_TIME  3

bool IsReady(unsigned long &timer, uint32_t ms) {
  if (millis() - timer < ms) return false;
  timer = millis();
  return true;
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  // in trạng thái đầu tiên
  Serial.printf("LED [RED] ON => %d Seconds\n", RED_TIME);
}

void loop() {
  static unsigned long timer = 0;
  static bool ledOn = false;

  static int state = 0;          // 0: RED, 1: GREEN, 2: YELLOW
  static int remaining = RED_TIME;

  if (IsReady(timer, 500)) {   // mỗi 1 giây

    ledOn = !ledOn;

    // tắt tất cả
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);

    // bật theo state
    if (state == 0) digitalWrite(PIN_LED_RED, ledOn);
    if (state == 1) digitalWrite(PIN_LED_GREEN, ledOn);
    if (state == 2) digitalWrite(PIN_LED_YELLOW, ledOn);

    // chỉ giảm thời gian khi LED vừa TẮT
    if (!ledOn) {
      remaining--;

      // hết thời gian → chuyển đèn
      if (remaining == 0) {

        if (state == 0) {
          state = 1;
          remaining = GREEN_TIME;
          Serial.printf("LED [GREEN] ON => %d Seconds", GREEN_TIME);
        }
        else if (state == 1) {
          state = 2;
          remaining = YELLOW_TIME;
          Serial.printf("LED [YELLOW] ON => %d Seconds", YELLOW_TIME);
        }
        else {
          state = 0;
          remaining = RED_TIME;
          Serial.printf("LED [RED] ON => %d Seconds", RED_TIME);
        }
      }
    }
  }
}
