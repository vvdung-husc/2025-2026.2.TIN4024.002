#include <Arduino.h>
#include <TM1637Display.h>

// LED
#define RED 32
#define YELLOW 33
#define GREEN 25

// Button
#define BUTTON 4

// LDR
#define LDR 34

// TM1637
#define CLK 18
#define DIO 19

TM1637Display display(CLK, DIO);

void countdown(int t) {
  for (int i = t; i >= 0; i--) {
    display.showNumberDec(i);
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  display.setBrightness(7);
}

void loop() {

  int light = analogRead(LDR);
  Serial.println(light);

  digitalWrite(RED, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);
  countdown(5);

  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(GREEN, LOW);
  countdown(3);

  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, HIGH);
  countdown(7);

  if (digitalRead(BUTTON) == LOW) {
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
    countdown(10);
  }
}