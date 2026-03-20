 #include <Arduino.h>
#include <TM1637Display.h>

// Định nghĩa chân cắm (theo sơ đồ của bạn)
#define RED_PIN 27
#define YELLOW_PIN 26
#define GREEN_PIN 25
#define PED_LED_PIN 21    // Đèn cho người đi bộ
#define BUTTON_PIN 23     // Nút bấm người đi bộ
#define SENSOR_PIN 5      // Cảm biến ánh sáng (D0)
#define CLK 18            // Chân CLK màn hình
#define DIO 19            // Chân DIO màn hình

TM1637Display display(CLK, DIO);

// Thời gian các pha (giây)
const int GREEN_TIME = 15;
const int YELLOW_TIME = 3;
const int RED_TIME = 25;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(PED_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Nút bấm dùng trở kéo lên nội bộ
  pinMode(SENSOR_PIN, INPUT);

  display.setBrightness(0x0f); // Độ sáng tối đa cho màn hình
  Serial.begin(115200);
}

// Hàm điều khiển đèn
void setLights(bool r, bool y, bool g, bool p) {
  digitalWrite(RED_PIN, r);
  digitalWrite(YELLOW_PIN, y);
  digitalWrite(GREEN_PIN, g);
  digitalWrite(PED_LED_PIN, p);
}

// Hàm đếm ngược và hiển thị lên LED 7 đoạn
void countdown(int seconds, String mode) {
  for (int i = seconds; i >= 0; i--) {
    display.showNumberDec(i);
    
    // Nếu đang đèn xanh mà có người bấm nút, có thể ưu tiên chuyển pha nhanh hơn
    if (mode == "GREEN" && digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("Co người đi bộ! Chuyển đèn sớm...");
      delay(500);
      return; 
    }
    
    delay(1000);
  }
}

void loop() {
  // Kiểm tra cảm biến ánh sáng (Chế độ ban đêm)
  // Nếu trời tối (giá trị cảm biến LOW/HIGH tùy module), có thể cho đèn vàng nhấp nháy
  if (digitalRead(SENSOR_PIN) == HIGH) { 
    Serial.println("Chế độ ban đêm: Đèn vàng nhấp nháy");
    setLights(LOW, HIGH, LOW, LOW);
    display.clear();
    delay(500);
    setLights(LOW, LOW, LOW, LOW);
    delay(500);
    return; // Thoát vòng lặp để chỉ chạy chế độ ban đêm
  }

  // CHU KỲ BÌNH THƯỜNG:
  
  // 1. Đèn Xanh (Xe đi)
  Serial.println("Đèn XANH");
  setLights(LOW, LOW, HIGH, LOW);
  countdown(GREEN_TIME, "GREEN");

  // 2. Đèn Vàng (Chuẩn bị dừng)
  Serial.println("Đèn VÀNG");
  setLights(LOW, HIGH, LOW, LOW);
  countdown(YELLOW_TIME, "YELLOW");

  // 3. Đèn Đỏ (Xe dừng - Người đi bộ được đi)
  Serial.println("Đèn ĐỎ");
  setLights(HIGH, LOW, LOW, HIGH); // Bật đèn Đỏ + Đèn người đi bộ
  countdown(RED_TIME, "RED");
}