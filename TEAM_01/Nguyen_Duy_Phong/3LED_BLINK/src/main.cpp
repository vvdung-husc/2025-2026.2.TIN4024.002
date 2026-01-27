#include <Arduino.h>

// Định nghĩa các chân GPIO
#define LED_RED 25
#define LED_YELLOW 26
#define LED_GREEN 27

// Thời gian
#define BLINK_TIME 500     // Nháy mỗi 0.5 giây
#define LED_DURATION 3000  // Mỗi đèn hoạt động trong 3 giây

// Biến đếm thời gian
unsigned long startTime = 0;
unsigned long currentTime = 0;

void blinkLED(int ledPin, String ledName) {
  unsigned long ledStartTime = millis();
  currentTime = (millis() - startTime) / 1000;
  
  Serial.print(currentTime);
  Serial.print("s - LED ");
  Serial.print(ledName);
  Serial.println(" bat dau nhay");
  
  // Nháy liên tục trong 3 giây
  while (millis() - ledStartTime < LED_DURATION) {
    digitalWrite(ledPin, HIGH);
    delay(BLINK_TIME);
    digitalWrite(ledPin, LOW);
    delay(BLINK_TIME);
  }
}

void setup() {
  // Khởi tạo Serial Monitor
  Serial.begin(115200);
  
  // Cấu hình các chân LED là OUTPUT
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  
  // Tắt tất cả LED ban đầu
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
  
  // Lưu thời điểm bắt đầu
  startTime = millis();
  
  Serial.println("=== HE THONG LED NHAY ===");
  Serial.println("Moi den nhay lien tuc 0.5s trong 3 giay");
  Serial.println("Cach nhau 3 giay\n");
}

void loop() {
  // LED ĐỎ nháy
  currentTime = (millis() - startTime) / 1000;
  blinkLED(LED_RED, "DO");
  
  // LED VÀNG nháy
  currentTime = (millis() - startTime) / 1000;
  blinkLED(LED_YELLOW, "VANG");
  
  // LED XANH nháy
  currentTime = (millis() - startTime) / 1000;
  blinkLED(LED_GREEN, "XANH");
  
  // Reset thời gian về 0 sau khi hết 3 LED
  Serial.println("\n--- Lap lai chu ky ---\n");
  startTime = millis();
}