/*
THÔNG TIN NHÓM 1
1. Đào Hữu Khánh
2. Lê Quang Duy
3. Trần Thị Như Sang
4. Nguyễn Duy Phong
5. Lê Trọng Kha

Dự án: Hệ thống giám sát nhiệt độ và độ ẩm với cảnh báo LED
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Cấu hình màn hình OLED với I2C tùy chỉnh
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define OLED_SDA 13      // Chân SDA theo diagram
#define OLED_SCL 12      // Chân SCL theo diagram

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Cấu hình DHT22
#define DHTPIN 16        // Chân kết nối DHT22 (GPIO 16)
#define DHTTYPE DHT22    // Loại cảm biến DHT22
DHT dht(DHTPIN, DHTTYPE);

// Định nghĩa chân LED (theo diagram.json)
#define LED_GREEN  15    // LED xanh lá (cyan)
#define LED_YELLOW 2     // LED vàng (qua R2)
#define LED_RED    4     // LED đỏ (qua R1)

// Biến toàn cục
float temperature = 0.0;
float humidity = 0.0;
unsigned long previousDHTMillis = 0;
unsigned long previousBlinkMillis = 0;
const long dhtInterval = 2000;     // Đọc DHT mỗi 2 giây
const long blinkInterval = 500;    // Nhấp nháy mỗi 0.5 giây

bool ledState = false;             // Trạng thái LED (bật/tắt)
String tempStatus = "";            // Trạng thái nhiệt độ (text)
String tempIcon = "";              // Icon nhiệt độ

// Khai báo hàm
void readDHT22();
void updateLEDs();
void updateDisplay();
void turnOffAllLEDs();
void determineTempStatus();

void setup() {
  // Khởi tạo Serial
  Serial.begin(115200);
  Serial.println("=== Hệ Thống Cảnh Báo Nhiệt Độ ===");
  
  // Cấu hình chân LED
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  
  // Tắt tất cả LED
  turnOffAllLEDs();
  
  // Khởi tạo I2C với chân tùy chỉnh
  Wire.begin(OLED_SDA, OLED_SCL);
  
  // Khởi tạo màn hình OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Lỗi khởi tạo OLED!"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Temperature");
  display.setCursor(10, 35);
  display.println("Alert System");
  display.display();
  delay(2000);
  
  // Khởi tạo DHT22
  dht.begin();
  
  Serial.println("Hệ thống đã sẵn sàng!");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Đọc cảm biến DHT22
  if (currentMillis - previousDHTMillis >= dhtInterval) {
    previousDHTMillis = currentMillis;
    readDHT22();
    determineTempStatus();
  }
  
  // Nhấp nháy LED
  if (currentMillis - previousBlinkMillis >= blinkInterval) {
    previousBlinkMillis = currentMillis;
    ledState = !ledState;
    updateLEDs();
  }
  
  // Cập nhật màn hình OLED
  updateDisplay();
  
  delay(1);
}

// Hàm đọc cảm biến DHT22
void readDHT22() {
  float newTemp = dht.readTemperature();
  float newHum = dht.readHumidity();
  
  // Kiểm tra nếu đọc thành công
  if (!isnan(newTemp) && !isnan(newHum)) {
    temperature = newTemp;
    humidity = newHum;
    
    Serial.print("Nhiệt độ: ");
    Serial.print(temperature, 2);
    Serial.print("°C | Độ ẩm: ");
    Serial.print(humidity, 2);
    Serial.print("% | Trạng thái: ");
    Serial.println(tempStatus);
  } else {
    Serial.println("Lỗi đọc DHT22!");
  }
}

// Hàm xác định trạng thái nhiệt độ
void determineTempStatus() {
  if (temperature < 13) {
    tempStatus = "TOO COLD";
    tempIcon = "**";  // ❄️
  } else if (temperature >= 13 && temperature < 20) {
    tempStatus = "COLD";
    tempIcon = "~~";  // 🌧️
  } else if (temperature >= 20 && temperature < 25) {
    tempStatus = "COOL";
    tempIcon = "--";  // ☁️
  } else if (temperature >= 25 && temperature < 30) {
    tempStatus = "WARM";
    tempIcon = "o";   // 🌤️
  } else if (temperature >= 30 && temperature < 35) {
    tempStatus = "HOT";
    tempIcon = "O";   // 🌞
  } else {
    tempStatus = "TOO HOT";
    tempIcon = "!!";  // 🔥
  }
}

// Hàm cập nhật LED theo nhiệt độ
void updateLEDs() {
  turnOffAllLEDs();
  
  if (!ledState) {
    return; // Tắt LED trong chu kỳ nhấp nháy
  }
  
  // Bật LED theo ngưỡng nhiệt độ
  if (temperature < 13) {
    // < 13°C: GREEN (TOO COLD)
    digitalWrite(LED_GREEN, HIGH);
  } else if (temperature >= 13 && temperature < 20) {
    // 13-20°C: GREEN (COLD)
    digitalWrite(LED_GREEN, HIGH);
  } else if (temperature >= 20 && temperature < 25) {
    // 20-25°C: YELLOW (COOL)
    digitalWrite(LED_YELLOW, HIGH);
  } else if (temperature >= 25 && temperature < 30) {
    // 25-30°C: YELLOW (WARM)
    digitalWrite(LED_YELLOW, HIGH);
  } else if (temperature >= 30 && temperature < 35) {
    // 30-35°C: RED (HOT)
    digitalWrite(LED_RED, HIGH);
  } else {
    // > 35°C: RED (TOO HOT)
    digitalWrite(LED_RED, HIGH);
  }
}

// Hàm cập nhật màn hình OLED
void updateDisplay() {
  display.clearDisplay();
  
  // Tiêu đề
  display.setTextSize(1);
  display.setCursor(15, 0);
  display.println("TEMP MONITOR");
  
  // Vẽ đường kẻ
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  
  // Hiển thị nhiệt độ
  display.setTextSize(1);
  display.setCursor(0, 15);
  display.println("Temperature:");
  
  display.setTextSize(2);
  display.setCursor(10, 25);
  if (temperature < 10) {
    display.print(" ");
  }
  display.print(temperature, 1);
  display.print(" C");
  
  // Vẽ đường kẻ
  display.drawLine(0, 42, 128, 42, SSD1306_WHITE);
  
  // Hiển thị độ ẩm
  display.setTextSize(1);
  display.setCursor(0, 46);
  display.print("Humidity: ");
  display.print(humidity, 1);
  display.println("%");
  
  // Hiển thị trạng thái
  display.setCursor(0, 56);
  display.print(tempIcon);
  display.print(" ");
  display.print(tempStatus);
  
  display.display();
}

// Hàm tắt tất cả LED
void turnOffAllLEDs() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
}
