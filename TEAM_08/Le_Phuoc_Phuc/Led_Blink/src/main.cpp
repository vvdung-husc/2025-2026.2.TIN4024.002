#include <Arduino.h>

// ================== CHÂN LED ==================
#define LED_RED     25
#define LED_YELLOW  33
#define LED_GREEN   32

// ================== THỜI GIAN (giây) ==================
int timeRed    = 5;   // LED đỏ nhấp nháy 5 giây
int timeYellow = 3;   // LED vàng nhấp nháy 3 giây
int timeGreen  = 7;   // LED xanh nhấp nháy 7 giây

// ================== HÀM NHẤP NHÁY + ĐẾM NGƯỢC ==================
void blinkLedWithCountdown(int ledPin, int seconds, const char* ledName) {
  Serial.println("---------------------------");
  Serial.print("Bat dau LED ");
  Serial.println(ledName);

  for (int i = seconds - 1; i >= 0; i--) {
    digitalWrite(ledPin, HIGH);
    Serial.print("LED ");
    Serial.print(ledName);
    Serial.print(" - Thoi gian con lai: ");
    Serial.print(i);
    Serial.println(" giay");

    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }

  Serial.print("LED ");
  Serial.print(ledName);
  Serial.println(" ket thuc");
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  Serial.println("ESP32 BAT DAU CHUONG TRINH");
}

// ================== LOOP ==================
void loop() {
  blinkLedWithCountdown(LED_RED, timeRed, "DO");
  delay(1000);

  blinkLedWithCountdown(LED_YELLOW, timeYellow, "VANG");
  delay(1000);

  blinkLedWithCountdown(LED_GREEN, timeGreen, "XANH");
  delay(2000);
}
