#include <Arduino.h>
#include <TM1637Display.h>

// --- Cấu hình chân màn hình TM1637 ---
#define CLK 2
#define DIO 3
TM1637Display display(CLK, DIO);

// --- Cấu hình chân Đèn giao thông ---
#define RED_LED 25
#define YELLOW_LED 26
#define GREEN_LED 27

// --- Cấu hình chân Nút nhấn ---
#define BUTTON_PIN 14

// --- Cấu hình cảm biến LDR & Đèn đường ---
#define LDR_PIN 34
#define STREET_LIGHT_PIN 32

// Các biến lưu trạng thái hoạt động
int state = 0; // 0: Đỏ, 1: Xanh, 2: Vàng
int timer = 5; // Thời gian đếm ngược (giây)
unsigned long previousMillis = 0;

bool displayEnabled = true; // Trạng thái màn hình (bật/tắt)
bool lastButtonState = HIGH; 

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo các chân Output cho đèn
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(STREET_LIGHT_PIN, OUTPUT);
  
  // Nút nhấn dùng điện trở kéo lên nội bộ (pull-up)
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Cảm biến ánh sáng
  pinMode(LDR_PIN, INPUT);

  // Thiết lập độ sáng màn hình TM1637 (mức 0x0f là sáng nhất)
  display.setBrightness(0x0f); 
  
  // Trạng thái ban đầu: Đèn Đỏ sáng, các đèn khác tắt
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(STREET_LIGHT_PIN, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Xử lý nút nhấn để Bật/Tắt hiển thị thời gian trên màn hình
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButtonState == HIGH) {
    displayEnabled = !displayEnabled; // Đảo trạng thái bật/tắt
    if (!displayEnabled) {
      display.clear(); // Xóa sạch màn hình nếu bị tắt
    }
    delay(50); // Độ trễ ngắn để chống dội phím (debounce)
  }
  lastButtonState = buttonState;

  // 2. Xử lý Cảm biến ánh sáng (Bật/Tắt đèn đường)
  int lightValue = analogRead(LDR_PIN);
  
  // Giá trị ADC của ESP32 từ 0-4095. Khi trời tối, giá trị sẽ giảm xuống thấp.
  // Ngưỡng 1000 có thể tùy chỉnh lại khi bạn test thực tế.
  if (lightValue < 1000) {
    digitalWrite(STREET_LIGHT_PIN, HIGH); // Bật đèn đường
  } else {
    digitalWrite(STREET_LIGHT_PIN, LOW);  // Tắt đèn đường
  }

  // 3. Xử lý logic Đèn giao thông và Đếm ngược (Mỗi 1 giây = 1000ms)
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;

    // Hiển thị thời gian nếu màn hình đang được phép bật
    if (displayEnabled) {
      display.showNumberDec(timer, true, 2, 2); // Căn lề phải, hiển thị 2 chữ số
    }

    timer--; // Giảm thời gian xuống 1 giây mỗi lần lặp

    // Đổi trạng thái đèn khi hết thời gian đếm ngược
    if (timer < 0) {
      if (state == 0) { // Đang Đỏ -> Chuyển sang Xanh
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        state = 1;
        timer = 5; // Set 5 giây cho đèn Xanh
      } 
      else if (state == 1) { // Đang Xanh -> Chuyển sang Vàng
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(YELLOW_LED, HIGH);
        state = 2;
        timer = 2; // Set 2 giây cho đèn Vàng
      } 
      else if (state == 2) { // Đang Vàng -> Chuyển sang Đỏ
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        state = 0;
        timer = 5; // Set 5 giây cho đèn Đỏ
      }
    }
  }
}