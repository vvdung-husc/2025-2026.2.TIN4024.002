#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 0
#define DIO 2
#define PIN_LED_RED 25
#define PIN_LED_YELLOW 33
#define PIN_LED_GREEN 32
      
TM1637Display display(CLK, DIO);  

// Non-blocking
bool IsReady(unsigned long &ulTimer, uint32_t millisecond) {
  if (millis() - ulTimer < millisecond) return false;
  ulTimer = millis();
  return true;
}

void setup() {
  Serial.begin(115200);
  display.setBrightness(0x0f);
  
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  printf("LED [RED   ] ON => 5 Seconds\n");
  display.showNumberDec(5);
}

void loop() {
  static unsigned long ulTimer = 0;
  static unsigned long countTimer = 0;
  static int state = 0; // 0: RED, 1: GREEN, 2: YELLOW
  static int timeLeft = 5;

  // 1. Điều khiển bật/tắt LED theo trạng thái
  digitalWrite(PIN_LED_RED,    (state == 0) ? HIGH : LOW);
  digitalWrite(PIN_LED_GREEN,  (state == 1) ? HIGH : LOW);
  digitalWrite(PIN_LED_YELLOW, (state == 2) ? HIGH : LOW);

  // 2. Cập nhật bộ đếm giây và hiển thị TM1637
  if (IsReady(countTimer, 1000)) {
    display.showNumberDec(timeLeft);
    if (timeLeft > 0) timeLeft--;
  }

  // 3. Logic chuyển đổi trạng thái đèn
  switch (state) {
    case 0: // ĐỎ - 5 giây
      if (IsReady(ulTimer, 5000)) {
        state = 1;
        timeLeft = 7;
        printf("LED [GREEN ] ON => 7 Seconds\n");
        display.showNumberDec(timeLeft); 
      }
      break;

    case 1: // XANH - 7 giây
      if (IsReady(ulTimer, 7000)) {
        state = 2;
        timeLeft = 3;
        printf("LED [YELLOW] ON => 3 Seconds\n");
        display.showNumberDec(timeLeft);
      }
      break;

    case 2: // VÀNG - 3 giây
      if (IsReady(ulTimer, 3000)) {
        state = 0;
        timeLeft = 5;
        printf("LED [RED   ] ON => 5 Seconds\n");
        display.showNumberDec(timeLeft);
      }
      break;
  }
}