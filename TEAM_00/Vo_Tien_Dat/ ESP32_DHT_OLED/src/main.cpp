#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 12, /* data=*/ 13);

#define DHTPIN 16 
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Định nghĩa chân LED
const int LED_CYAN   = 15;  
const int LED_YELLOW = 2;  
const int LED_RED    = 4;  

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 DHT OLED...");

  pinMode(LED_CYAN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  dht.begin();
  u8g2.begin();
  Serial.println("OLED initialized");
  delay(100); // Đợi OLED ổn định
}

void loop() {
  Serial.println("Loop start");
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Sensor read failed!");
    // Hiển thị lỗi trên OLED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 30, "Sensor Error");
    u8g2.sendBuffer();
    delay(2000);
    return;
  }

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" C, Hum: ");
  Serial.print(hum);
  Serial.println(" %");

  String statusText = "";
  String ledColor = "";
  int activeLED = -1;

  // Xác định trạng thái và đèn LED dựa trên bảng yêu cầu
  if (temp < 13.0) { statusText = "TOO COLD"; ledColor = "Cyan"; activeLED = LED_CYAN; }
  else if (temp < 20.0) { statusText = "COLD"; ledColor = "Cyan"; activeLED = LED_CYAN; }
  else if (temp < 25.0) { statusText = "COOL"; ledColor = "Yellow"; activeLED = LED_YELLOW; }
  else if (temp < 30.0) { statusText = "WARM"; ledColor = "Yellow"; activeLED = LED_YELLOW; }
  else if (temp < 35.0) { statusText = "HOT"; ledColor = "Red"; activeLED = LED_RED; }
  else { statusText = "TOO HOT"; ledColor = "Red"; activeLED = LED_RED; }

  // --- HIỂN THỊ OLED ---
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // Dòng 1: Nhiệt độ
  u8g2.drawStr(0, 10, "Temp: ");
  char tempBuf[10];
  sprintf(tempBuf, "%.2f", temp);
  u8g2.drawStr(40, 10, tempBuf);
  u8g2.drawStr(80, 10, "°C");

  // Dòng 2: Độ ẩm
  u8g2.drawStr(0, 22, "Hum: ");
  char humBuf[10];
  sprintf(humBuf, "%.2f", hum);
  u8g2.drawStr(40, 22, humBuf);
  u8g2.drawStr(80, 22, "%");

  // Dòng 3: Trạng thái
  u8g2.drawStr(0, 34, "Status: ");
  u8g2.drawStr(50, 34, statusText.c_str());

  // Dòng 4: LED
  u8g2.drawStr(0, 46, "LED: ");
  u8g2.drawStr(40, 46, ledColor.c_str());

  u8g2.sendBuffer();

  // --- HIỆU ỨNG NHẤP NHÁY LED ---
  digitalWrite(LED_CYAN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  if (activeLED != -1) {
    digitalWrite(activeLED, HIGH);
    delay(500); 
    digitalWrite(activeLED, LOW);
    delay(500); 
  }

  delay(1000); // Cập nhật mỗi 1 giây
}