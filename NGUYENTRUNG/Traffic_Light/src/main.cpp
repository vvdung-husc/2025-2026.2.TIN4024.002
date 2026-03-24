#include <Arduino.h>
#include <TM1637Display.h>

// LED
#define RED_LED    25
#define YELLOW_LED 26
#define GREEN_LED  27
#define BLUE_LED   14

// BUTTON
#define BUTTON_PIN 33

// LDR
#define LDR_PIN 34

// TM1637
#define CLK 18
#define DIO 19

TM1637Display display(CLK, DIO);

void showCountdown(int seconds) {
  for (int i = seconds; i >= 0; i--) {
    display.showNumberDec(i, true);
    delay(500);
  }
}

void setup() {

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  display.setBrightness(7);

  Serial.begin(115200);
}

void loop() {

  int light = analogRead(LDR_PIN);

  if (light < 1500) {
    digitalWrite(BLUE_LED, HIGH);
  } else {
    digitalWrite(BLUE_LED, LOW);
  }

  if (digitalRead(BUTTON_PIN) == LOW) {

    digitalWrite(RED_LED, HIGH);
    showCountdown(5);
    digitalWrite(RED_LED, LOW);
  }

  digitalWrite(RED_LED, HIGH);
  showCountdown(10);
  digitalWrite(RED_LED, LOW);

  digitalWrite(GREEN_LED, HIGH);
  showCountdown(7);
  digitalWrite(GREEN_LED, LOW);

  digitalWrite(YELLOW_LED, HIGH);
  showCountdown(3);
  digitalWrite(YELLOW_LED, LOW);
}