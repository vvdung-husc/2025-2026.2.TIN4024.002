#include <Arduino.h>

void setup() {
  // Chân 13 thường là chân đèn LED có sẵn trên mạch Uno
  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH); // Bật đèn
  delay(1000);            // Đợi 1 giây
  digitalWrite(13, LOW);  // Tắt đèn
  delay(1000);            // Đợi 1 giây
}