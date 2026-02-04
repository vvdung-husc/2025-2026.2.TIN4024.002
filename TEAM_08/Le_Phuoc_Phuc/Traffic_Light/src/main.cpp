#include <Arduino.h>
#include <TM1637Display.h>

// ================= LED =================
#define LED_RED     25
#define LED_YELLOW  33
#define LED_GREEN   32
#define LED_BLUE    21

// ================= LDR =================
#define LDR_PIN     13

// ================= DISPLAY =================
#define CLK 18
#define DIO 19
TM1637Display display(CLK, DIO);

// ================= THỜI GIAN (giây) =================
int timeRed    = 5;
int timeYellow = 3;
int timeGreen  = 7;

// ================= NGƯỠNG ÁNH SÁNG =================
int lightThreshold = 2000;  // >2000 = sáng

// ================= HÀM NHẤP NHÁY + ĐẾM NGƯỢC =================
void blinkLed(int ledPin, int seconds, const char* name) {
  Serial.println("-------------------------");
  Serial.print("Bat dau LED ");
  Serial.println(name);

  for (int i = seconds - 1; i >= 0; i--) {
    display.showNumberDec(i, true);  // hiển thị đếm ngược

    Serial.print("LED ");
    Serial.print(name);
    Serial.print(" - Con lai: ");
    Serial.print(i);
    Serial.println(" giay");

    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  pinMode(LDR_PIN, INPUT);

  display.setBrightness(7);
  display.clear();

  Serial.println("ESP32 + LDR + LED + DISPLAY BAT DAU");
}

// ================= LOOP =================
void loop() {
  int lightValue = analogRead(LDR_PIN);

  Serial.print("Gia tri LDR: ");
  Serial.println(lightValue);

  // ===== TRỜI SÁNG → TẮT HỆ THỐNG =====
  if (lightValue > lightThreshold) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    display.clear();

    Serial.println("Moi truong SANG → Tat LED");
    delay(1000);
    return;
  }

  // ===== TRỜI TỐI → CHẠY HỆ THỐNG =====
  Serial.println("Moi truong TOI → He thong hoat dong");

  digitalWrite(LED_BLUE, HIGH); // báo hệ thống ON

  blinkLed(LED_RED, timeRed, "DO");
  delay(500);

  blinkLed(LED_YELLOW, timeYellow, "VANG");
  delay(500);

  blinkLed(LED_GREEN, timeGreen, "XANH");
  delay(1000);
}
