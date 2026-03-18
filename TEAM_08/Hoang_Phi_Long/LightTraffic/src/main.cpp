#include <Arduino.h>
#include <TM1637Display.h>

// ================= PIN =================
#define LED_RED     25
#define LED_YELLOW  26
#define LED_GREEN   27
#define LED_BLUE    14

#define BUTTON_PIN  12
#define LDR_PIN     34

#define CLK 18
#define DIO 19

TM1637Display display(CLK, DIO);

// =============== TIME ==================
int greenTime  = 10;
int yellowTime = 3;
int redTime    = 10;

// ======================================
void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  display.setBrightness(7);
  display.clear();

  Serial.begin(115200);
}

// ================= LED CONTROL =========
void setLights(bool r, bool y, bool g) {
  digitalWrite(LED_RED, r);
  digitalWrite(LED_YELLOW, y);
  digitalWrite(LED_GREEN, g);
}

// ================= COUNTDOWN ===========
void countdown(int seconds) {
  for (int i = seconds; i >= 0; i--) {
    display.showNumberDec(i, true);
    delay(1000);

    // Nếu nhấn nút → sang đèn đỏ
    if (digitalRead(BUTTON_PIN) == LOW) {
      display.clear();
      return;
    }
  }
}

// ================= LOOP ================
void loop() {

  // ===== LDR =====
  int ldrValue = analogRead(LDR_PIN);
  Serial.println(ldrValue);

  if (ldrValue < 1500) {        // Trời tối
    digitalWrite(LED_BLUE, HIGH);
  } else {
    digitalWrite(LED_BLUE, LOW);
  }

  // ===== ĐÈN XANH =====
  setLights(LOW, LOW, HIGH);
  countdown(greenTime);

  // ===== ĐÈN VÀNG =====
  setLights(LOW, HIGH, LOW);
  countdown(yellowTime);

  // ===== ĐÈN ĐỎ =====
  setLights(HIGH, LOW, LOW);
  countdown(redTime);
}