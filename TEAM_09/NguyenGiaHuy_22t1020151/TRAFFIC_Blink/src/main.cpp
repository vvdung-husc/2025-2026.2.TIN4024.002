#include <Arduino.h>

// GPIO theo diagram.json
#define LED_RED     25
#define LED_YELLOW  33
#define LED_GREEN   32

// Thời gian từng đèn (giây)
#define RED_TIME     5
#define YELLOW_TIME  3
#define GREEN_TIME   7

unsigned long lastTick = 0;
unsigned long lastPrint = 0;
int remaining = 0;
uint8_t state = 0;

// non-blocking timer
bool IsReady(unsigned long &t, unsigned long ms) {
  if (millis() - t < ms) return false;
  t = millis();
  return true;
}

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  Serial.begin(115200);
  Serial.println("TRAFFIC LIGHT START");

  // trạng thái ban đầu
  state = 0;
  remaining = RED_TIME;
  lastTick = millis();
  lastPrint = millis();
}

void loop() {

  /* In ra mỗi 1 giây */
  if (IsReady(lastPrint, 1000)) {
    remaining--;

    switch (state) {
      case 0:
        Serial.printf("LED [RED] ON => %d Seconds\n", remaining);
        break;
      case 1:
        Serial.printf("LED [GREEN] ON => %d Seconds\n", remaining);
        break;
      case 2:
        Serial.printf("LED [YELLOW] ON => %d Seconds\n", remaining);
        break;
    }
  }

  /* Điều khiển đèn theo state */
  switch (state) {

    case 0: // RED
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, LOW);

      if (remaining <= 0) {
        state = 1;
        remaining = GREEN_TIME;
      }
      break;

    case 1: // GREEN
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, HIGH);

      if (remaining <= 0) {
        state = 2;
        remaining = YELLOW_TIME;
      }
      break;

    case 2: // YELLOW
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_GREEN, LOW);

      if (remaining <= 0) {
        state = 0;
        remaining = RED_TIME;
      }
      break;
  }
}
