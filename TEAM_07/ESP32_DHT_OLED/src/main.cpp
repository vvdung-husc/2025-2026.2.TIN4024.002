/*
THÔNG TIN NHÓM 7
1. Lê Công Mẫn
2. Võ Tiến Đạt
3. Hồ Thị Kiều Oanh 
4. Nguyễn Phan Thủy Tiên
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- 1. CẤU HÌNH CHÂN (GIỮ NGUYÊN THEO SƠ ĐỒ CŨ) ---
#define OLED_SDA 13
#define OLED_SCL 12
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 16     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

// Định nghĩa đèn (Lưu ý: Đèn Cyan trong hình sẽ đóng vai trò là đèn GREEN)
#define LED_GREEN  15  // Dây màu xanh ngọc
#define LED_YELLOW 2   // Dây màu vàng
#define LED_RED    4   // Dây màu đỏ

void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  dht.begin();
  
  // Khởi tạo OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED Error")); for(;;);
  }
  display.setTextColor(WHITE);
}

// Hàm hỗ trợ tắt hết đèn
void turnOffAllLeds() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
}

// Hàm làm đèn nhấp nháy
void blinkLed(int pin) {
  digitalWrite(pin, HIGH);
  delay(200); // Sáng 0.2 giây
  digitalWrite(pin, LOW);
  delay(200); // Tắt 0.2 giây
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read DHT!"));
    return;
  }

  // --- 2. XỬ LÝ LOGIC THEO BẢNG NHIỆT ĐỘ ---
  String status = "";
  int ledToBlink = -1; // -1 nghĩa là không đèn nào sáng

  if (t < 13) {
    status = "TOO COLD";
    ledToBlink = LED_GREEN;
  } 
  else if (t < 20) {
    status = "COLD";
    ledToBlink = LED_GREEN;
  }
  else if (t < 25) {
    status = "COOL";
    ledToBlink = LED_YELLOW;
  }
  else if (t < 30) {
    status = "WARM";
    ledToBlink = LED_YELLOW;
  }
  else if (t < 35) {
    status = "HOT";
    ledToBlink = LED_RED;
  }
  else { // t >= 35
    status = "TOO HOT";
    ledToBlink = LED_RED;
  }

  // --- 3. ĐIỀU KHIỂN ĐÈN NHẤP NHÁY ---
  turnOffAllLeds(); // Tắt hết các đèn khác
  if (ledToBlink != -1) {
    blinkLed(ledToBlink); // Chỉ nhấp nháy đèn được chọn
  }

  // --- 4. HIỂN THỊ LÊN MÀN HÌNH ---
  display.clearDisplay();

  // Dòng 1: Tên nhóm
  display.setCursor(0, 0); 
  display.setTextSize(1);
  display.print("NHOM 7: ");
  display.print(status); // In trạng thái (COLD, HOT...)

  // Dòng 2: Nhiệt độ
  display.setCursor(0, 16); 
  display.setTextSize(2);
  display.print(t); 
  display.setTextSize(1);
  display.cp437(true); 
  display.write(167); display.print("C"); // Dấu độ C

  // Dòng 3: Độ ẩm
  display.setCursor(0, 40);
  display.setTextSize(1);
  display.print("Humidity: ");
  display.print(h);
  display.print(" %");

  display.display();
}