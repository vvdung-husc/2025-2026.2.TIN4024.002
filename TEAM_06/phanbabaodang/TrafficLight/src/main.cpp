#include <Arduino.h>

#define LED_RED     25
#define LED_YELLOW  33
#define LED_GREEN   32
#define LED_BLUE    21
#define BUTTON_PIN  23

#define TM_CLK 15
#define TM_DIO 2

#define RED_TIME     5000
#define GREEN_TIME   5000
#define YELLOW_TIME  2000


enum TrafficState {
  RED_STATE,
  GREEN_STATE,
  YELLOW_STATE
};

TrafficState state = RED_STATE;
unsigned long lastTime = 0;


void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);



  digitalWrite(LED_RED, HIGH);
}

void loop() {
  unsigned long now = millis();
  int remain = 0;


  if (digitalRead(BUTTON_PIN) == LOW) {
    state = RED_STATE;
    lastTime = now;
  }

  switch (state) {
    case RED_STATE:
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, LOW);

      remain = (RED_TIME - (now - lastTime)) / 1000;
      if (now - lastTime >= RED_TIME) {
        state = GREEN_STATE;
        lastTime = now;
      }
      break;

    case GREEN_STATE:
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_BLUE, LOW);

      remain = (GREEN_TIME - (now - lastTime)) / 1000;
      if (now - lastTime >= GREEN_TIME) {
        state = YELLOW_STATE;
        lastTime = now;
      }
      break;

    case YELLOW_STATE:
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_GREEN, LOW);

      digitalWrite(LED_BLUE, (now / 300) % 2);

      remain = (YELLOW_TIME - (now - lastTime)) / 1000;
      if (now - lastTime >= YELLOW_TIME) {
        state = RED_STATE;
        lastTime = now;
        digitalWrite(LED_BLUE, LOW);
      }
      break;
  }

  if (remain < 0) remain = 0;
}
