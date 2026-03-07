/*
 * Dự án: Hệ thống Đèn Giao Thông Thông Minh
 * Board: ESP32 DevKit V4
 * Tác giả: phong315
 * 
 * Chức năng:
 * - Đèn giao thông tự động (Đỏ -> Vàng -> Xanh)
 * - Hiển thị đếm ngược trên màn hình 7 đoạn TM1637
 * - Nút nhấn: Bật/tắt hiển thị thời gian đếm ngược
 * - Cảm biến LDR: Bật/tắt đèn giao thông theo cường độ ánh sáng
 * - LED xanh dương: Đèn đường (tự động bật khi tối)
 */
#include <Arduino.h>
#include <TM1637Display.h>


// Định nghĩa chân kết nối
#define LED_RED    27    // LED đỏ
#define LED_YELLOW 26    // LED vàng
#define LED_GREEN  25    // LED xanh lá
#define LED_BLUE   21    // LED xanh dương (chế độ ban đêm)

#define BUTTON     23    // Nút nhấn (người đi bộ)
#define LDR_PIN    13    // Cảm biến ánh sáng

#define CLK_PIN    18    // Chân CLK của TM1637
#define DIO_PIN    19    // Chân DIO của TM1637

// Khởi tạo màn hình 7 đoạn
TM1637Display display(CLK_PIN, DIO_PIN);

// Thời gian cho mỗi đèn (tính bằng giây)
#define RED_TIME    10   // Đèn đỏ: 10 giây
#define YELLOW_TIME 3    // Đèn vàng: 3 giây
#define GREEN_TIME  10   // Đèn xanh: 10 giây

// Trạng thái đèn giao thông
enum TrafficState {
  STATE_RED,
  STATE_YELLOW,
  STATE_GREEN
};

// Biến toàn cục
TrafficState currentState = STATE_RED;
unsigned long previousMillis = 0;
int countdown = RED_TIME;

// Biến cho nút nhấn
bool displayEnabled = true;      // Trạng thái hiển thị đếm ngược
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Biến cho cảm biến ánh sáng
int lightLevel = 0;
bool trafficLightOn = true;      // Trạng thái đèn giao thông
bool streetLightOn = false;      // Trạng thái đèn đường (LED xanh dương)
const int LIGHT_THRESHOLD = 2500; // Ngưỡng ánh sáng (có thể điều chỉnh)

// Khai báo hàm (Forward declarations)
void turnOffAllLEDs();
void checkButton();
void checkLightSensor();
void handleTrafficLight();
void changeState();
void updateTrafficLight();
void printCurrentState();

void setup() {
  // Khởi tạo Serial để debug
  Serial.begin(115200);
  Serial.println("=== Hệ Thống Đèn Giao Thông ===");
  
  // Cấu hình chân LED là OUTPUT
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  
  // Cấu hình nút nhấn với pull-up
  pinMode(BUTTON, INPUT_PULLUP);
  
  // Cấu hình cảm biến ánh sáng
  pinMode(LDR_PIN, INPUT);
  
  // Khởi tạo màn hình 7 đoạn
  display.setBrightness(0x0f); // Độ sáng tối đa
  display.clear();
  
  // Tắt tất cả LED
  turnOffAllLEDs();
  
  // Bắt đầu với đèn đỏ
  digitalWrite(LED_RED, HIGH);
  
  Serial.println("Hệ thống đã sẵn sàng!");
  Serial.println("Nhấn nút để bật/tắt hiển thị đếm ngược");
  Serial.println("LDR sẽ tự động bật/tắt đèn giao thông");
}

void loop() {
  // Đọc trạng thái nút nhấn (bật/tắt hiển thị)
  checkButton();
  
  // Đọc cảm biến ánh sáng (bật/tắt đèn giao thông)
  checkLightSensor();
  
  // Xử lý đèn giao thông chỉ khi được bật
  if (trafficLightOn) {
    handleTrafficLight();
  }
  
  // Hiển thị thời gian đếm ngược nếu được bật
  if (displayEnabled && trafficLightOn) {
    display.showNumberDec(countdown, false);
  } else {
    display.clear();
  }
  
  delay(10); // Delay nhỏ để tránh đọc nút quá nhanh
}

// Hàm kiểm tra nút nhấn (bật/tắt hiển thị đếm ngược)
void checkButton() {
  bool currentButtonState = digitalRead(BUTTON);
  
  // Phát hiện nhấn nút với debounce
  if (currentButtonState == LOW && !buttonPressed) {
    unsigned long currentTime = millis();
    
    if ((currentTime - lastDebounceTime) > debounceDelay) {
      buttonPressed = true;
      lastDebounceTime = currentTime;
      
      // Đảo trạng thái hiển thị
      displayEnabled = !displayEnabled;
      
      if (displayEnabled) {
        Serial.println("Hiển thị đếm ngược: BẬT");
      } else {
        Serial.println("Hiển thị đếm ngược: TẮT");
        display.clear();
      }
    }
  } else if (currentButtonState == HIGH) {
    buttonPressed = false;
  }
}

// Hàm kiểm tra cảm biến ánh sáng (bật/tắt đèn giao thông)
void checkLightSensor() {
  lightLevel = analogRead(LDR_PIN);
  
  // Nếu tối (giá trị LDR cao)
  if (lightLevel > LIGHT_THRESHOLD) {
    // Bật đèn đường (LED xanh dương)
    if (!streetLightOn) {
      streetLightOn = true;
      digitalWrite(LED_BLUE, HIGH);
      Serial.println(">>> Đèn đường: BẬT (Đã tối)");
    }
    
    // Tắt đèn giao thông
    if (trafficLightOn) {
      trafficLightOn = false;
      turnOffAllLEDs();
      digitalWrite(LED_BLUE, HIGH); // Giữ đèn đường bật
      display.clear();
      Serial.println(">>> Đèn giao thông: TẮT (Đã tối)");
    }
  } 
  // Nếu sáng (giá trị LDR thấp)
  else {
    // Tắt đèn đường
    if (streetLightOn) {
      streetLightOn = false;
      digitalWrite(LED_BLUE, LOW);
      Serial.println(">>> Đèn đường: TẮT (Đã sáng)");
    }
    
    // Bật đèn giao thông
    if (!trafficLightOn) {
      trafficLightOn = true;
      // Bật lại đèn theo trạng thái hiện tại
      updateTrafficLight();
      Serial.println(">>> Đèn giao thông: BẬT (Đã sáng)");
    }
  }
  
  // In giá trị ánh sáng mỗi 2 giây
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    Serial.print("Cường độ ánh sáng: ");
    Serial.print(lightLevel);
    Serial.print(" | Ngưỡng: ");
    Serial.println(LIGHT_THRESHOLD);
    lastPrint = millis();
  }
}

// Hàm xử lý đèn giao thông
void handleTrafficLight() {
  unsigned long currentMillis = millis();
  
  // Cập nhật đếm ngược mỗi giây
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    countdown--;
    
    // In trạng thái hiện tại
    printCurrentState();
    
    // Kiểm tra nếu hết thời gian
    if (countdown <= 0) {
      changeState();
    }
  }
}

// Hàm thay đổi trạng thái đèn
void changeState() {
  switch (currentState) {
    case STATE_RED:
      currentState = STATE_GREEN;
      countdown = GREEN_TIME;
      turnOffAllLEDs();
      digitalWrite(LED_GREEN, HIGH);
      Serial.println(">>> Chuyển sang: ĐÈN XANH");
      break;
      
    case STATE_GREEN:
      currentState = STATE_YELLOW;
      countdown = YELLOW_TIME;
      turnOffAllLEDs();
      digitalWrite(LED_YELLOW, HIGH);
      Serial.println(">>> Chuyển sang: ĐÈN VÀNG");
      break;
      
    case STATE_YELLOW:
      currentState = STATE_RED;
      countdown = RED_TIME;
      turnOffAllLEDs();
      digitalWrite(LED_RED, HIGH);
      Serial.println(">>> Chuyển sang: ĐÈN ĐỎ");
      break;
  }
}

// Hàm cập nhật đèn giao thông theo trạng thái hiện tại
void updateTrafficLight() {
  turnOffAllLEDs();
  switch (currentState) {
    case STATE_RED:
      digitalWrite(LED_RED, HIGH);
      break;
    case STATE_YELLOW:
      digitalWrite(LED_YELLOW, HIGH);
      break;
    case STATE_GREEN:
      digitalWrite(LED_GREEN, HIGH);
      break;
  }
}

// Hàm tắt tất cả LED giao thông
void turnOffAllLEDs() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
}

// Hàm in trạng thái hiện tại
void printCurrentState() {
  Serial.print("Trạng thái: ");
  switch (currentState) {
    case STATE_RED:
      Serial.print("ĐỎ");
      break;
    case STATE_YELLOW:
      Serial.print("VÀNG");
      break;
    case STATE_GREEN:
      Serial.print("XANH");
      break;
  }
  Serial.print(" | Đếm ngược: ");
  Serial.print(countdown);
  Serial.print(" giây");
  Serial.print(" | Hiển thị: ");
  Serial.println(displayEnabled ? "BẬT" : "TẮT");
}
