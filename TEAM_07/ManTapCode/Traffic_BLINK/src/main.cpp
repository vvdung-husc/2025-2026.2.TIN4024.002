#include <Arduino.h>

// Định nghĩa các chân cắm (theo hình ảnh bạn cung cấp)
const int RED_LED = 25;
const int YELLOW_LED = 33; 
const int GREEN_LED = 32;

void setup() {
  // Cấu hình các chân LED là OUTPUT
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  // Đặt trạng thái ban đầu là tắt
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  
  // Khởi tạo Serial để theo dõi trạng thái giống như trong hình
  Serial.begin(115200);
}

// Hàm để nhấp nháy đèn theo số lần và thời gian yêu cầu
// Hàm để nhấp nháy đèn theo số lần và thời gian yêu cầu
void blinkLED(int pin, int times, const char* colorName) {
  Serial.print("LED [");
  Serial.print(colorName);
  Serial.print("] ON => ");
  Serial.print(times);
  Serial.println(" Seconds (Blinking)");

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH); // Bật đèn
    delay(500);              // Đợi 0.5 giây
    digitalWrite(pin, LOW);  // Tắt đèn
    delay(500);              // Đợi 0.5 giây (Tổng cộng 1 giây/lần)
  }
}

void loop() {
  // 1. Đèn Đỏ nhấp nháy 5 lần trong 5 giây
  blinkLED(RED_LED, 5, "RED");

  // 2. Đèn Vàng nhấp nháy 3 lần trong 3 giây (theo Terminal trong hình)
  blinkLED(YELLOW_LED, 3, "YELLOW");

  // 3. Đèn Xanh nhấp nháy 7 lần trong 7 giây (theo Terminal trong hình)
  blinkLED(GREEN_LED, 7, "GREEN");
}