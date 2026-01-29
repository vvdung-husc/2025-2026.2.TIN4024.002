#include <Arduino.h>

// ================= GPIO =================
#define PIN_LED_RED     25
#define PIN_LED_YELLOW  33
#define PIN_LED_GREEN   32
#define PIN_LED_BLUE    21   // LED xanh dương báo trạng thái hiển thị

#define PIN_BTN_DISPLAY 23   // nút xanh

#define TM_CLK 18
#define TM_DIO 19

// ============== TIME (seconds) ==========
#define RED_TIME     5
#define GREEN_TIME   7
#define YELLOW_TIME  3

// ================= TM1637 DRIVER =================
void tmStart() {
  digitalWrite(TM_DIO, HIGH);
  digitalWrite(TM_CLK, HIGH);
  digitalWrite(TM_DIO, LOW);
}

void tmStop() {
  digitalWrite(TM_CLK, LOW);
  digitalWrite(TM_DIO, LOW);
  digitalWrite(TM_CLK, HIGH);
  digitalWrite(TM_DIO, HIGH);
}

void tmWriteByte(uint8_t data) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(TM_CLK, LOW);
    digitalWrite(TM_DIO, data & 0x01);
    data >>= 1;
    digitalWrite(TM_CLK, HIGH);
  }
  digitalWrite(TM_CLK, LOW);
  pinMode(TM_DIO, INPUT);
  digitalWrite(TM_CLK, HIGH);
  pinMode(TM_DIO, OUTPUT);
}

const uint8_t segMap[] = {
  0x3f, 0x06, 0x5b, 0x4f,
  0x66, 0x6d, 0x7d, 0x07,
  0x7f, 0x6f
};

void tmDisplayNumber(int num) {
  tmStart();
  tmWriteByte(0x40);
  tmStop();

  tmStart();
  tmWriteByte(0xC0);
  tmWriteByte(segMap[num / 10]);
  tmWriteByte(segMap[num % 10]);
  tmWriteByte(0x00);
  tmWriteByte(0x00);
  tmStop();

  tmStart();
  tmWriteByte(0x88 | 0x07); // brightness max
  tmStop();
}

// ================= TIMER =================
bool IsReady(unsigned long &timer, uint32_t ms) {
  if (millis() - timer < ms) return false;
  timer = millis();
  return true;
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);

  pinMode(PIN_BTN_DISPLAY, INPUT_PULLUP);

  pinMode(TM_CLK, OUTPUT);
  pinMode(TM_DIO, OUTPUT);

  digitalWrite(PIN_LED_BLUE, HIGH); // mặc định bật hiển thị

  Serial.printf("\rLED [RED] ON => %d Seconds   ", RED_TIME);
}

// ================= LOOP =================
void loop() {
  static unsigned long timer = 0;
  static bool ledOn = false;

  static int state = 0;          // 0: RED, 1: GREEN, 2: YELLOW
  static int remaining = RED_TIME;

  static bool showDisplay = true;
  static bool lastBtn = HIGH;

  // ===== BUTTON TOGGLE DISPLAY + BLUE LED =====
  bool btn = digitalRead(PIN_BTN_DISPLAY);
  if (btn == LOW && lastBtn == HIGH) {
    showDisplay = !showDisplay;
    digitalWrite(PIN_LED_BLUE, showDisplay ? HIGH : LOW);
  }
  lastBtn = btn;

  // ===== MAIN TIMER (1 second blink) =====
  if (IsReady(timer, 500)) {   // 500ms để tạo nháy 1s ON / 1s OFF
    ledOn = !ledOn;

    // tắt tất cả đèn giao thông
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);

    // bật đèn theo state
    if (state == 0) digitalWrite(PIN_LED_RED, ledOn);
    if (state == 1) digitalWrite(PIN_LED_GREEN, ledOn);
    if (state == 2) digitalWrite(PIN_LED_YELLOW, ledOn);

    // hiển thị đếm ngược
    if (showDisplay) {
      tmDisplayNumber(remaining);
    }

    // giảm thời gian khi LED vừa TẮT
    if (!ledOn) {
      remaining--;

      if (remaining == 0) {
        if (state == 0) {
          state = 1;
          remaining = GREEN_TIME;
          Serial.printf("\rLED [GREEN] ON => %d Seconds   ", GREEN_TIME);
        }
        else if (state == 1) {
          state = 2;
          remaining = YELLOW_TIME;
          Serial.printf("\rLED [YELLOW] ON => %d Seconds  ", YELLOW_TIME);
        }
        else {
          state = 0;
          remaining = RED_TIME;
          Serial.printf("\rLED [RED] ON => %d Seconds     ", RED_TIME);
        }
      }
    }
  }
}
