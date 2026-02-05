#include <Arduino.h>

#define LED1 25
#define LED2 26
#define LED3 27

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop() {
  digitalWrite(LED1, HIGH);
  delay(400);
  digitalWrite(LED1, LOW);

  digitalWrite(LED2, HIGH);
  delay(400);
  digitalWrite(LED2, LOW);

  digitalWrite(LED3, HIGH);
  delay(400);
  digitalWrite(LED3, LOW);
}
