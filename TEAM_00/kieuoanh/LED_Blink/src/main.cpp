#include <Arduino.h>

#define PIN_LED_RED 23

// Hàm kiểm tra thời gian (Non-blocking)
bool IsReady(unsigned long &ulTimer, uint32_t millisecond) {
  if (millis() - ulTimer < millisecond) return false;
  ulTimer = millis();
  return true;
}

void setup() {
  Serial.begin(115200); // Khởi động Serial Monitor
  printf("WELCOME IOT\n");
  pinMode(PIN_LED_RED, OUTPUT);
}

void loop() {
  static unsigned long ulTimer = 0;
  static bool ledStatus = false;

  // Cứ mỗi 500ms thì đổi trạng thái đèn 1 lần
  if (IsReady(ulTimer, 500)){
    ledStatus = !ledStatus;
    printf("LED IS %s\n", ledStatus ? "ON" : "OFF");
    digitalWrite(PIN_LED_RED, ledStatus ? HIGH : LOW);
  }
}