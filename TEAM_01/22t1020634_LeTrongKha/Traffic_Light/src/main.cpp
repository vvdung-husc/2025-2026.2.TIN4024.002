/*
 * Dự án: Hệ thống Đèn Giao Thông Thông Minh
 * Board: ESP32 DevKit V4
 * Tác giả: Lê Trọng Kha - 22t1020634
 * 
 * Chức năng:
 * - Đèn giao thông tự động (Đỏ -> Vàng -> Xanh)
 * - Hiển thị đếm ngược trên màn hình 7 đoạn TM1637
 * - Nút nhấn: Bật/tắt hiển thị thời gian đếm ngược
 * - Cảm biến LDR: Bật/tắt đèn giao thông theo cường độ ánh sáng
 * - LED xanh dương: Đèn đường (tự động bật khi tối)
 * 
 * Mô phỏng hệ thống giao thông thông minh với khả năng tự động điều chỉnh theo môi trường
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
bool displayEnabled = true;
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Biến cho cảm biến ánh sáng
int lightLevel = 0;
bool trafficLightOn = true;
bool streetLightOn = false;
const int LIGHT_THRESHOLD = 2500;

// Khai báo hàm
void turnOffAllLEDs();
void checkButton();
void checkLightSensor();
void handleTrafficLight();
void changeState();
void updateTrafficLight();
void printCurrentState();

void setup() {
  Serial.begin(115200);
  Serial.println("=== Hệ Thống Đèn Giao Thông ===");
  
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);
  
  display.setBrightness(0x0f);
  display.clear();
  
  turnOffAllLEDs();
  digitalWrite(LED_RED, HIGH);
  
  Serial.println("Hệ thống đã sẵn sàng!");
}

void loop() {
  checkButton();
  checkLightSensor();
  
  if (trafficLightOn) {
    handleTrafficLight();
  }
  
  if (displayEnabled && trafficLightOn) {
    display.showNumberDec(countdown, false);
  } else {
    display.clear();
  }
  
  delay(10);
}

void checkButton() {
  bool currentButtonState = digitalRead(BUTTON);
  
  if (currentButtonState == LOW && !buttonPressed) {
    unsigned long currentTime = millis();
    
    if ((currentTime - lastDebounceTime) > debounceDelay) {
      buttonPressed = true;
      lastDebounceTime = currentTime;
      displayEnabled = !displayEnabled;
      
      Serial.println(displayEnabled ? "Hiển thị: BẬT" : "Hiển thị: TẮT");
      if (!displayEnabled) display.clear();
    }
  } else if (currentButtonState == HIGH) {
    buttonPressed = false;
  }
}

void checkLightSensor() {
  lightLevel = analogRead(LDR_PIN);
  
  if (lightLevel > LIGHT_THRESHOLD) {
    if (!streetLightOn) {
      streetLightOn = true;
      digitalWrite(LED_BLUE, HIGH);
      Serial.println(">>> Đèn đường: BẬT");
    }
    
    if (trafficLightOn) {
      trafficLightOn = false;
      turnOffAllLEDs();
      digitalWrite(LED_BLUE, HIGH);
      display.clear();
      Serial.println(">>> Đèn giao thông: TẮT");
    }
  } else {
    if (streetLightOn) {
      streetLightOn = false;
      digitalWrite(LED_BLUE, LOW);
      Serial.println(">>> Đèn đường: TẮT");
    }
    
    if (!trafficLightOn) {
      trafficLightOn = true;
      updateTrafficLight();
      Serial.println(">>> Đèn giao thông: BẬT");
    }
  }
}

void handleTrafficLight() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    countdown--;
    printCurrentState();
    
    if (countdown <= 0) {
      changeState();
    }
  }
}

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

void turnOffAllLEDs() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
}

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
  Serial.println(" giây");
}
