#include <Arduino.h>

#define RED 15
#define YELLOW 2
#define GREEN 4

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
}

void loop() {

  // Green
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
  delay(5000);

  // Yellow
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, HIGH);
  delay(2000);

  // Red
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, HIGH);
  delay(5000);
}