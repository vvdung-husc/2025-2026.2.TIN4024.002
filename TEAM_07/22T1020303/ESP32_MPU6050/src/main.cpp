#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LED_PIN 5 // Thêm chân LED đỏ ở đây

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); 
  pinMode(LED_PIN, OUTPUT); // Khai báo LED là đầu ra

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;); 
  }

  mpu.initialize();
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  ax = -ax; // Giữ nguyên đảo ngược giá trị AX của bạn

  // --- PHẦN LED ĐỎ ---
  // Nếu nghiêng quá mức (trị tuyệt đối của ax > 10000) thì đèn sáng
  if (abs(ax) > 10000) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // --- HIỂN THỊ OLED ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("MPU6050 + LED RED");
  
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.print("AX: ");
  display.print(ax);
  
  display.display(); 
  delay(200);
}