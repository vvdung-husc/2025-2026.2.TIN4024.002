#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 18
#define DIO 19
#define PIN_LED_RED 27
#define PIN_LED_YELLOW 26
#define PIN_LED_GREEN 25
#define PIN_LED_BLUE 21  //đèn trạng thái
#define PIN_BUTTON 23
#define PIN_LDR 13

const float GAMMA = 0.7;
const float RL10 = 50;

TM1637Display display(CLK, DIO);

bool systemOn = true;
int state = 0; // 0: RED, 1: GREEN, 2: YELLOW
int timeLeft = 5;
unsigned long ulTimer = 0;
unsigned long countTimer = 0;

const char* stateNames[] = {"RED", "GREEN", "YELLOW"};

//Non-blocking
bool IsReady(unsigned long &ulTimer, uint32_t millisecond) {
  if (millis() - ulTimer < millisecond) return false;
  ulTimer = millis();
  return true;
}

//AnalogRead: đọc cường độ ánh sáng
float getLux() {
  int analogValue = analogRead(PIN_LDR);
  // ESP32 ADC là 12-bit (4095) và điện áp 3.3V
  float voltage = analogValue / 4095.0 * 3.3;
  float resistance = 2000 * voltage / (1 - voltage / 3.3);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
  return lux;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  display.setBrightness(0x0f);
  
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  printf("LED [%s] ON => %d Seconds\n", stateNames[state], timeLeft);
}

void loop() {
  // put your main code here, to run repeatedly:
  //1.Bật/tắt hệ thống
  static bool lastBtnState = HIGH;
  bool currentBtnState = digitalRead(PIN_BUTTON);
  if (lastBtnState == HIGH && currentBtnState == LOW) {
    systemOn = !systemOn;
    delay(50); 
    if (!systemOn) {
      digitalWrite(PIN_LED_RED, LOW);
      digitalWrite(PIN_LED_YELLOW, LOW);
      digitalWrite(PIN_LED_GREEN, LOW);
      digitalWrite(PIN_LED_BLUE, LOW);
      display.clear();
    }
  }
  lastBtnState = currentBtnState;

  if (!systemOn) return;

  //2.Đọc cảm biến ánh sáng
  float lux = getLux();
  bool isNightMode = (lux < 2000);
  

  //3.Điều khiển đèn
  digitalWrite(PIN_LED_BLUE, HIGH); 

  if (isNightMode) {
    // CHẾ ĐỘ BAN ĐÊM: Đèn Vàng sáng liên tục, tắt các đèn khác
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, HIGH);
    
    if (IsReady(countTimer, 1000)) {
        display.clear(); 
        
    }
    ulTimer = millis(); // Reset timer để giữ nguyên trạng thái khi trời sáng lại
  } 
  else {
    // CHẾ ĐỘ BÌNH THƯỜNG: Sáng đứng theo trạng thái
    digitalWrite(PIN_LED_RED,    (state == 0) ? HIGH : LOW);
    digitalWrite(PIN_LED_GREEN,  (state == 1) ? HIGH : LOW);
    digitalWrite(PIN_LED_YELLOW, (state == 2) ? HIGH : LOW);
  }
  //4.
  if (IsReady(countTimer, 1000)) {
    display.showNumberDec(timeLeft);
    printf(" [%s] => seconds: %d \n", stateNames[state], timeLeft);
    if (timeLeft > 0) timeLeft--;
  }

  //5.
  switch (state) {
    case 0: // ĐỎ
      if (IsReady(ulTimer, 5000)) {
        state = 1; timeLeft = 7;
        display.showNumberDec(timeLeft);
        printf("LED [%s] ON => %d Seconds\n", stateNames[state], timeLeft);
      }
      break;
    case 1: // XANH
      if (IsReady(ulTimer, 7000)) {
        state = 2; timeLeft = 3;
        display.showNumberDec(timeLeft);
        printf("LED [%s] ON => %d Seconds\n", stateNames[state], timeLeft);
      }
      break;
    case 2: // VÀNG
      if (IsReady(ulTimer, 3000)) {
        state = 0; timeLeft = 5;
        display.showNumberDec(timeLeft);
        printf("LED [%s] ON => %d Seconds\n", stateNames[state], timeLeft);
      }
      break;
  }
}
