#include <Arduino.h>
#include <TM1637Display.h>

// ====== CHÂN LED ======
#define RED     25
#define YELLOW  26
#define GREEN   27

// ====== TM1637 ======
#define CLK 18
#define DIO 19
TM1637Display display(CLK, DIO);

// ====== THỜI GIAN (giây) ======
#define RED_TIME     10
#define YELLOW_TIME  3
#define GREEN_TIME   10

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);

  display.setBrightness(7);
  display.clear();
}

// hàm đếm ngược và hiển thị
void countdown(int timeSec) {
  for (int i = timeSec; i >= 0; i--) {
    display.showNumberDec(i, true);
    delay(1000);
  }
}

void loop() {
  // ====== ĐÈN ĐỎ ======
  digitalWrite(RED, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);
  countdown(RED_TIME);

  // ====== ĐÈN XANH ======
  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, HIGH);
  countdown(GREEN_TIME);

  // ====== ĐÈN VÀNG ======
  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(GREEN, LOW);
  countdown(YELLOW_TIME);
}
