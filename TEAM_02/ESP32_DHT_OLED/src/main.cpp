/*
THÔNG TIN NHÓM 02
1. Mai Anh Quân
2. Lê Văn Quốc Dũng
3. Nguyễn Văn Khải
4. Nguyễn Trọng Vĩ
*/

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHTPIN 15
#define DHTTYPE DHT22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {

  Serial.begin(115200);
  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    for(;;);
  }

  display.clearDisplay();
}

void loop() {

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,10);
  display.print("Temp: ");
  display.print(temp);
  display.println(" C");

  display.setCursor(0,30);
  display.print("Hum: ");
  display.print(hum);
  display.println(" %");

  display.display();

  delay(2000);
}