#include <Arduino.h>

void setup() {
  // Bạn phải dùng đúng chân 23 như trong diagram.json
  pinMode(23, OUTPUT); 
}

void loop() {
  digitalWrite(23, HIGH); // Bật LED
  delay(500);
  digitalWrite(23, LOW);  // Tắt LED
  delay(500);
}