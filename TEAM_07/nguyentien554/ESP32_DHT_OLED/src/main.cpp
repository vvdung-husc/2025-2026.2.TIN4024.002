#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Cấu hình OLED 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Cấu hình DHT22
#define DHTPIN 16
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Cấu hình LED 
#define LED_GREEN 15
#define LED_YELLOW 2
#define LED_RED 4

// Non-blocking
unsigned long prevDHTTime = 0;
unsigned long prevBlinkTime = 0;
bool ledVisible = false;

// Hàm kiểm tra thời gian
bool IsReady(unsigned long &timer, uint32_t interval) {
  if (millis() - timer < interval) return false;
  timer = millis();
  return true;
}

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo chân I2C tùy chỉnh cho OLED
  Wire.begin(13, 12); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed"));
    for(;;);
  }
  
  dht.begin();
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  static float t = 0, h = 0;
  static String statusMsg = "";
  static int currentLed = -1;

  // 1. Đọc cảm biến mỗi 2 giây
  if (IsReady(prevDHTTime, 2000)) {
    t = dht.readTemperature();
    h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      statusMsg = "ERR SENSOR";
    } else {
      // Logic phân loại ngưỡng 
      if (t < 13) { statusMsg = "TOO COLD"; currentLed = LED_GREEN; }
      else if (t < 20) { statusMsg = "COLD"; currentLed = LED_GREEN; }
      else if (t < 25) { statusMsg = "COOL"; currentLed = LED_YELLOW; }
      else if (t < 30) { statusMsg = "WARM"; currentLed = LED_YELLOW; }
      else if (t < 35) { statusMsg = "HOT"; currentLed = LED_RED; }
      else { statusMsg = "TOO HOT"; currentLed = LED_RED; }
    }

    // Hiển thị OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Temp: "); display.print(t); display.println(" C");
    display.print("Humi: "); display.print(h); display.println(" %");
    display.drawFastHLine(0, 20, 128, SSD1306_WHITE);
    
    display.setTextSize(2);
    display.setCursor(0, 35);
    display.println(statusMsg);
    display.display();
  }

  // 2. Logic nhấp nháy đèn mỗi 1 giây (500ms ON / 500ms OFF)
  if (IsReady(prevBlinkTime, 500)) {
    ledVisible = !ledVisible;
    
    // Tắt tất cả LED
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);

    // Bật LED tương ứng theo trạng thái nhấp nháy
    if (currentLed != -1 && ledVisible) {
      digitalWrite(currentLed, HIGH);
    }
  }
}