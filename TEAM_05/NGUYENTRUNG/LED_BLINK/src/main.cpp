#include <Arduino.h>

int ledXanh = 25;  
int ledVang = 26;  
int ledDo   = 27;  

void setup() {
  Serial.begin(115200);
  pinMode(ledXanh, OUTPUT);
  pinMode(ledVang, OUTPUT);
  pinMode(ledDo, OUTPUT);
}

void blinkLed(int pin, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
    delay(500);
  }
}

void loop() {
  Serial.println("RED");
  blinkLed(ledDo, 5);

  Serial.println("YELLOW");
  blinkLed(ledVang, 3);

  Serial.println("GREEN");
  blinkLed(ledXanh, 7);
}
