#include <Arduino.h>

// Định nghĩa các chân cắm tương ứng với diagram.json
const int RED_LED = 2;
const int YELLOW_LED = 4;
const int GREEN_LED = 5;

void setup() {
  // Thiết lập các chân là đầu ra
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
}

void loop() {
  // Đèn Đỏ sáng trong 3 giây
  digitalWrite(RED_LED, HIGH);
  delay(3000); 
  digitalWrite(RED_LED, LOW);

  // Đèn Xanh sáng trong 3 giây
  digitalWrite(GREEN_LED, HIGH);
  delay(3000);
  digitalWrite(GREEN_LED, LOW);

  // Đèn Vàng sáng trong 1 giây
  digitalWrite(YELLOW_LED, HIGH);
  delay(1000);
  digitalWrite(YELLOW_LED, LOW);
}