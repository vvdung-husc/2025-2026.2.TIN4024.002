#include <Arduino.h>

int red = 32;
int yellow = 33;
int green = 25;

void setup() {
  Serial.begin(115200);

  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
}

void loop() {

  Serial.println("LED RED ON -> 5 Seconds");
  digitalWrite(red, HIGH);
  digitalWrite(yellow, LOW);
  digitalWrite(green, LOW);
  delay(5000);

  Serial.println("LED YELLOW ON -> 3 Seconds");
  digitalWrite(red, LOW);
  digitalWrite(yellow, HIGH);
  digitalWrite(green, LOW);
  delay(3000);

  Serial.println("LED GREEN ON -> 7 Seconds");
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(green, HIGH);
  delay(7000);

}