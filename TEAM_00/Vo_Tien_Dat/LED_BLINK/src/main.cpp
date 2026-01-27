#include <Arduino.h>

// Định nghĩa các chân cắm LED theo sơ đồ của bạn
const int RED_LED = 25;
const int YELLOW_LED = 26;
const int GREEN_LED = 27;

void setup() {
  // Cấu hình các chân là đầu ra
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  
  Serial.begin(115200);
}

void loop() {
  // Đèn ĐỎ sáng 5 giây
  Serial.println("LED [RED   ] ON => 5 Seconds");
  digitalWrite(RED_LED, HIGH);
  delay(500);
  digitalWrite(RED_LED, LOW);

  // Đèn VÀNG sáng 3 giây
  Serial.println("LED [YELLOW] ON => 3 Seconds");
  digitalWrite(YELLOW_LED, HIGH);
  delay(300);
  digitalWrite(YELLOW_LED, LOW);

  // Đèn XANH sáng 7 giây
  Serial.println("LED [GREEN ] ON => 7 Seconds");
  digitalWrite(GREEN_LED, HIGH);
  delay(700);
  digitalWrite(GREEN_LED, LOW);
}

// void loop() {
//   // put your main code here, to run repeatedly:
//   //static int i = 0;
//   //printf("Loop running ... %d\n",++i);
//   //delay(1000);
//   digitalWrite(PIN_LED_RED, HIGH); // LED ON
//   delay(50000); // Wait 500 ms
//   digitalWrite(PIN_LED_RED, LOW); // LED OFF
//   //delay(500); // Wait 500 ms

// }

