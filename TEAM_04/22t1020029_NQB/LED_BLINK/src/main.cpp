#include <Arduino.h>

void setup() {
  pinMode(23, OUTPUT); // Khai báo chân 23 là đầu ra
}

void loop() {
  digitalWrite(23, HIGH); // Bật LED
  delay(500);
  digitalWrite(23, LOW);  // Tắt LED
  delay(500);
}