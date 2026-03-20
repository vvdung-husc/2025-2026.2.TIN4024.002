#include <Arduino.h>
#include <TM1637Display.h>

// Khai báo chân LED
#define LED_RED    25
#define LED_YELLOW 33
#define LED_GREEN  32

// Chân TM1637
#define CLK 15
#define DIO 2

TM1637Display display(CLK, DIO);

// Hàm bật 1 đèn và đếm ngược
void runLight(int ledPin, int seconds) {
  // Tắt tất cả đèn
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);

  // Bật đèn hiện tại
  digitalWrite(ledPin, HIGH);

  // Đếm ngược
  for (int i = seconds; i >= 0; i--) {
    display.showNumberDec(i, true); // hiển thị số giây còn lại
    delay(1000);
  }
}

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  display.setBrightness(7); // độ sáng 0–7
  display.clear();
}

void loop() {
  // Đèn đỏ: 5 giây
  runLight(LED_RED, 5);

  // Đèn vàng: 3 giây
  runLight(LED_YELLOW, 3);

  // Đèn xanh: 7 giây
  runLight(LED_GREEN, 7);
}
