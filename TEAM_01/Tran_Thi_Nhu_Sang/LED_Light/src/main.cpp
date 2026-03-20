#include <Arduino.h>

#define RED_LED     26
#define YELLOW_LED  27
#define GREEN_LED   25

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  // 🔴 ĐỎ: 5 giây
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  Serial.println("RED ON - 5s");
  delay(5000);

  // 🟡 VÀNG: 3 giây
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  Serial.println("YELLOW ON - 3s");
  delay(3000);

  // 🟢 XANH: 7 giây
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  Serial.println("GREEN ON - 7s");
  delay(7000);
}
