#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IR1_PIN 4
#define IR2_PIN 5
#define LED_PIN 2

LiquidCrystal_I2C lcd(0x27, 16, 2);

int peopleCount = 0;

// Lưu cảm biến nào được kích hoạt trước
int firstSensor = 0; // 0 = chưa có, 1 = IR1, 2 = IR2
unsigned long firstTriggerTime = 0;

// Chống lặp xung
unsigned long lastIR1Trigger = 0;
unsigned long lastIR2Trigger = 0;

// Thời gian khóa để tránh 1 lần Send bị đếm nhiều lần
const unsigned long debounceTime = 800;

// Thời gian tối đa giữa 2 cảm biến để tính là 1 lượt đi qua
const unsigned long sequenceWindow = 2000;

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("People Counter");
  lcd.setCursor(0, 1);
  lcd.print("Count: ");
  lcd.print(peopleCount);
}

void printStatus(const char* msg) {
  Serial.println("--------------------------------");
  Serial.println(msg);
  Serial.print("So nguoi hien tai: ");
  Serial.println(peopleCount);
  Serial.println("--------------------------------");
}

void handleIR1() {
  unsigned long now = millis();

  // Chống lặp
  if (now - lastIR1Trigger < debounceTime) return;
  lastIR1Trigger = now;

  Serial.println("IR1 duoc kich hoat");

  if (firstSensor == 0) {
    firstSensor = 1;
    firstTriggerTime = now;
    Serial.println("Dang cho IR2 de xac dinh huong di...");
    return;
  }

  // Nếu IR2 đã kích trước đó -> người đi ra
  if (firstSensor == 2 && (now - firstTriggerTime <= sequenceWindow)) {
    if (peopleCount > 0) {
      peopleCount--;
    }
    updateLCD();
    printStatus("=> PHAT HIEN NGUOI DI RA");
    firstSensor = 0;
    firstTriggerTime = 0;
    return;
  }

  // Nếu không hợp lệ thì reset
  firstSensor = 1;
  firstTriggerTime = now;
  Serial.println("Reset chuoi, bat dau lai tu IR1");
}

void handleIR2() {
  unsigned long now = millis();

  // Chống lặp
  if (now - lastIR2Trigger < debounceTime) return;
  lastIR2Trigger = now;

  Serial.println("IR2 duoc kich hoat");

  if (firstSensor == 0) {
    firstSensor = 2;
    firstTriggerTime = now;
    Serial.println("Dang cho IR1 de xac dinh huong di...");
    return;
  }

  // Nếu IR1 đã kích trước đó -> người đi vào
  if (firstSensor == 1 && (now - firstTriggerTime <= sequenceWindow)) {
    peopleCount++;
    updateLCD();
    printStatus("=> PHAT HIEN NGUOI DI VAO");
    firstSensor = 0;
    firstTriggerTime = 0;
    return;
  }

  // Nếu không hợp lệ thì reset
  firstSensor = 2;
  firstTriggerTime = now;
  Serial.println("Reset chuoi, bat dau lai tu IR2");
}

void setup() {
  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  updateLCD();

  Serial.println("=== HE THONG DEM NGUOI BAT DAU ===");
  Serial.println("Cach test:");
  Serial.println("- Bam Send o IR1 roi bam Send o IR2 => Nguoi di vao");
  Serial.println("- Bam Send o IR2 roi bam Send o IR1 => Nguoi di ra");
  Serial.println();
}

void loop() {
  // Đọc tín hiệu từ 2 IR Receiver
  int ir1 = digitalRead(IR1_PIN);
  int ir2 = digitalRead(IR2_PIN);

  // IR receiver khi nhận tín hiệu thường kéo chân xuống LOW
  if (ir1 == LOW) {
    handleIR1();
    delay(50);
  }

  if (ir2 == LOW) {
    handleIR2();
    delay(50);
  }

  // Nếu chờ quá lâu mà không có cảm biến thứ hai thì reset
  if (firstSensor != 0 && (millis() - firstTriggerTime > sequenceWindow)) {
    Serial.println("Het thoi gian cho, reset chuoi dem.");
    firstSensor = 0;
    firstTriggerTime = 0;
  }

  // LED sáng khi có người
  digitalWrite(LED_PIN, peopleCount > 0 ? HIGH : LOW);
}