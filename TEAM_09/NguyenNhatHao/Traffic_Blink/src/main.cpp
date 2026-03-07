#include <Arduino.h>
#include <TM1637Display.h>

// Khai báo chân các đèn
#define RED_LED_PIN     23
#define YELLOW_LED_PIN  22
#define GREEN_LED_PIN   21
#define STREET_LED_PIN  4    // Đèn đường, bật khi trời tối

// TM1637
#define TM_CLK 18
#define TM_DIO 19

// Push Button
#define BUTTON_PIN 17

// LDR Analog pin (ESP32 ADC: A0 = 34)
#define LDR_PIN 34

// Thời gian mỗi pha (đơn vị: giây)
const uint8_t RED_TIME = 5;
const uint8_t GREEN_TIME = 5;
const uint8_t YELLOW_TIME = 2;
const int LDR_THRESHOLD = 1800; // Có thể cần điều chỉnh tùy thực tế, giá trị ADC đầu ra trời tối

// Khai báo
TM1637Display display(TM_CLK, TM_DIO);

bool display_on = true;

void IRAM_ATTR onButtonPress() {
  display_on = !display_on;
}

void setup() {
  // LED
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(STREET_LED_PIN, OUTPUT);

  // Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPress, FALLING);

  // TM1637 hiển thị
  display.setBrightness(0x0f);

  // LDR 
  pinMode(LDR_PIN, INPUT);
}

void loop() {
  // Kiểm tra LDR để điều khiển đèn đường
  int ldrValue = analogRead(LDR_PIN);
  if (ldrValue < LDR_THRESHOLD) {
    digitalWrite(STREET_LED_PIN, HIGH); // Trời tối, bật đèn đường
  } else {
    digitalWrite(STREET_LED_PIN, LOW);
  }

  // RED
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  for (int i = RED_TIME; i > 0; i--) {
    if (display_on) display.showNumberDecEx(i, 0, true);
    else display.clear();
    delay(1000);
  }

  // GREEN
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, HIGH);
  for (int i = GREEN_TIME; i > 0; i--) {
    if (display_on) display.showNumberDecEx(i, 0, true);
    else display.clear();
    delay(1000);
  }

  // YELLOW
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, LOW);
  for (int i = YELLOW_TIME; i > 0; i--) {
    if (display_on) display.showNumberDecEx(i, 0, true);
    else display.clear();
    delay(1000);
  }
}