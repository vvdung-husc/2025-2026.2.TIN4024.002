#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Chân mặc định ESP32: SDA=21, SCL=22

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Khong tim thay OLED SSD1306"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Initializing...");
  display.display();

  mpu.initialize();
  Serial.println(mpu.testConnection() ? "MPU6050 OK" : "MPU6050 FAIL");
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Đảo dấu trục X theo yêu cầu
  ax = -ax;

  // Hiển thị lên màn hình OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("MPU6050 + ESP32");
  
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.print("AX: ");
  display.println(ax);

  display.setTextSize(1);
  display.setCursor(0, 50);
  display.print(ax > 10000 ? "WARM: Tilted Left" : (ax < -10000 ? "WARM: Tilted Right" : "Status: Stable"));
  
  display.display();
  delay(200);
}