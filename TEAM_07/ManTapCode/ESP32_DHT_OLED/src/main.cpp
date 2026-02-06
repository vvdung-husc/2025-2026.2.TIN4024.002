#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- 1. CẤU HÌNH PHẦN CỨNG (PIN MAPPING) ---
// Chân màn hình OLED (I2C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Chân cảm biến DHT22
#define DHTPIN 15     // Chân Data nối vào GPIO 15
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

// Chân đèn LED
#define LED_GREEN  5   // Đèn Xanh (Lạnh)
#define LED_YELLOW 18  // Đèn Vàng (Bình thường)
#define LED_RED    19  // Đèn Đỏ (Nóng)

// Biến toàn cục
bool oledFound = false;

// Cấu trúc dữ liệu cho bảng nhiệt độ
struct TemperatureRange {
  float minTemp;
  float maxTemp;
  String tempRange;     // Hiển thị: "< 13°C"
  String statusText;    // HOT, COLD, etc
  String emoji;         // ❄️ ☔ ☁️ ☀️ 🌞 🔥 (hoặc ký tự thay thế)
  int ledPin;          // LED_GREEN, LED_YELLOW, LED_RED
};

// Bảng nhiệt độ
TemperatureRange tempRanges[] = {
  {-100, 13, "< 13°C",   "TOO COLD", "F",  LED_GREEN},
  {13,   20, "13-20°C",  "COLD",     "R",  LED_GREEN},
  {20,   25, "20-25°C",  "COOL",     "C",  LED_YELLOW},
  {25,   30, "25-30°C",  "WARM",     "S",  LED_YELLOW},
  {30,   35, "30-35°C",  "HOT",      "H",  LED_RED},
  {35,   100, "> 35°C",  "TOO HOT",  "X",  LED_RED}
};

int numRanges = sizeof(tempRanges) / sizeof(tempRanges[0]);

// Hàm tìm khoảng nhiệt độ
int findTemperatureRange(float temp) {
  for(int i = 0; i < numRanges; i++) {
    if(temp >= tempRanges[i].minTemp && temp < tempRanges[i].maxTemp) {
      return i;
    }
  }
  return numRanges - 1; // Mặc định: khoảng cuối cùng
}

// --- HÀM KIỂM TRA I2C DEVICES ---
void scanI2C() {
  byte error, address;
  int nDevices;

  Serial.println("\n=== QUÉT I2C BUS ===");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Tìm thiết bị I2C tại địa chỉ: 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  
  if (nDevices == 0)
    Serial.println("Không tìm thấy thiết bị I2C nào!");
  else
    Serial.println("=== KẾT THÚC QUÉT ===\n");
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Chờ Serial sẵn sàng
  
  Serial.println("\n\n========================================");
  Serial.println("=== BẮT ĐẦU KHỞI ĐỘNG HỆ THỐNG ===");
  Serial.println("========================================");

  // Cấu hình chân LED là đầu ra TRƯỚC
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  // Kiểm tra LED bằng cách bật lần lượt
  Serial.println("\n--- TEST LED ---");
  Serial.print("Bật LED GREEN (GPIO 5)... ");
  digitalWrite(LED_GREEN, HIGH);
  delay(300);
  digitalWrite(LED_GREEN, LOW);
  Serial.println("OK");
  
  Serial.print("Bật LED YELLOW (GPIO 18)... ");
  digitalWrite(LED_YELLOW, HIGH);
  delay(300);
  digitalWrite(LED_YELLOW, LOW);
  Serial.println("OK");
  
  Serial.print("Bật LED RED (GPIO 19)... ");
  digitalWrite(LED_RED, HIGH);
  delay(300);
  digitalWrite(LED_RED, LOW);
  Serial.println("OK");
  
  // Khởi động Wire (I2C) - GPIO 21 = SDA, GPIO 22 = SCL (ESP32 mặc định)
  Serial.println("\n--- KHỞI ĐỘNG I2C ---");
  Wire.begin(21, 22); // SDA=21, SCL=22 (ESP32 pins)
  Wire.setClock(100000); // 100kHz speed
  delay(100);
  Serial.println("I2C khởi động OK");
  
  // Quét các thiết bị I2C để tìm OLED
  Serial.println("\n--- QUÉT I2C DEVICES ---");
  scanI2C();

  // Khởi động cảm biến DHT
  Serial.println("\n--- KHỞI ĐỘNG DHT22 ---");
  dht.begin();
  Serial.println("DHT22 khởi động OK");

  // Khởi động màn hình OLED (Thử các địa chỉ phổ biến)
  Serial.println("\n--- KHỞI ĐỘNG OLED ---");
  oledFound = false;
  byte oledAddr[] = {0x3C, 0x3D, 0x78, 0x7A}; // Các địa chỉ có thể của SSD1306
  
  for(int i = 0; i < 4; i++) {
    Serial.print("Thử kết nối OLED tại 0x");
    if(oledAddr[i] < 16) Serial.print("0");
    Serial.println(oledAddr[i], HEX);
    
    if(display.begin(SSD1306_SWITCHCAPVCC, oledAddr[i])) {
      Serial.print("✓ OLED tìm thấy tại: 0x");
      if(oledAddr[i] < 16) Serial.print("0");
      Serial.println(oledAddr[i], HEX);
      
      // Test hiển thị
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(5, 20);
      display.println("OLED OK!");
      display.setCursor(5, 35);
      display.println("System Starting...");
      display.display();
      
      oledFound = true;
      break;
    }
  }
  
  if(!oledFound) {
    Serial.println("========================================");
    Serial.println("✗ KHÔNG TÌM THẤY OLED!");
    Serial.println("========================================");
    Serial.println("\nKiểm tra kết nối:");
    Serial.println("1. Nối SDA (GPIO 21) với pin SDA của OLED");
    Serial.println("2. Nối SCL (GPIO 22) với pin SCL của OLED");
    Serial.println("3. Nối GND với GND của OLED");
    Serial.println("4. Nối 3.3V với VCC của OLED");
    Serial.println("5. Có pull-up resistor 10k ohm không?");
    Serial.println("\nHệ thống sẽ tiếp tục chạy...");
    Serial.println("========================================\n");
  } else {
    Serial.println("✓ Tất cả thành phần khởi động OK!\n");
  }
}

void loop() {
  // --- BƯỚC 1: ĐỌC DỮ LIỆU ---
  // Chờ 2+ giây giữa các lần đọc (DHT22 cần tối thiểu 2 giây)
  delay(2500); 

  float t = NAN;
  float h = NAN;
  int attempts = 3; // Thử 3 lần nếu lỗi

  // Retry logic - thử đọc nhiều lần
  while (attempts > 0 && (isnan(h) || isnan(t))) {
    t = dht.readTemperature(); // Đọc nhiệt độ
    h = dht.readHumidity();    // Đọc độ ẩm
    
    if (isnan(h) || isnan(t)) {
      attempts--;
      if (attempts > 0) {
        delay(1000); // Chờ 1 giây trước khi retry
      }
    }
  }

  // Kiểm tra nếu đọc bị lỗi sau tất cả lần thử
  if (isnan(h) || isnan(t)) {
    Serial.println("Lỗi: Không đọc được dữ liệu DHT!");
    Serial.println("Kiểm tra:");
    Serial.println("1. Kết nối dây GPIO 15");
    Serial.println("2. Cảm biến DHT22 có cấp điện không?");
    Serial.println("3. Cảm biến bị lỗi không?");
    
    if(oledFound) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.println("LOI: Khong doc");
      display.println("duoc DHT22!");
      display.println("");
      display.println("Kiem tra:");
      display.println("- Noi day GPIO15");
      display.println("- Cap dien cam bien");
      display.display();
    }
    return;
  }

  // --- BƯỚC 2: XỬ LÝ LOGIC THEO BẢNG YÊU CẦU ---
  // Tìm khoảng nhiệt độ
  int rangeIndex = findTemperatureRange(t);
  
  String tempRange = tempRanges[rangeIndex].tempRange;
  String statusText = tempRanges[rangeIndex].statusText;
  String statusEmoji = tempRanges[rangeIndex].emoji;
  int activeLed = tempRanges[rangeIndex].ledPin;

  // --- BƯỚC 3: HIỂN THỊ LÊN OLED (NẾU CÓ) ---
  if(oledFound) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    
    // ===== DÒNG 1: Khoảng nhiệt độ + Trạng thái =====
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Temp: ");
    display.println(tempRange);
    
    // ===== DÒNG 2: Nhiệt độ hiện tại (to) =====
    display.setTextSize(2);
    display.setCursor(0, 12);
    display.print(t, 1);
    display.setTextSize(1);
    display.println("C");
    
    // ===== DÒNG 3: Độ ẩm =====
    display.setTextSize(1);
    display.setCursor(0, 32);
    display.print("Humidity: ");
    display.print(h, 0);
    display.println("%");
    
    // ===== DÒNG 4: Trạng thái + Ký hiệu + Màu LED =====
    display.setCursor(0, 45);
    display.print(statusText);
    display.print(" ");
    display.print(statusEmoji);
    
    // Hiển thị màu LED
    display.setCursor(100, 45);
    if(activeLed == LED_GREEN) {
      display.println("G");
    } else if(activeLed == LED_YELLOW) {
      display.println("Y");
    } else if(activeLed == LED_RED) {
      display.println("R");
    }
    
    display.display(); // Xuất ra màn hình
  }

  // --- BƯỚC 4: IN DỮ LIỆU LÊN SERIAL ---
  Serial.print("Temp: ");
  Serial.print(t, 1);
  Serial.print("°C | Range: ");
  Serial.print(tempRange);
  Serial.print(" | Humidity: ");
  Serial.print(h, 1);
  Serial.print("% | Status: ");
  Serial.print(statusText);
  Serial.print(" ");
  Serial.println(statusEmoji);

  // --- BƯỚC 5: ĐIỀU KHIỂN ĐÈN LED NHẤP NHÁY ---
  // Tắt tất cả đèn trước để đảm bảo chỉ đèn đúng mới sáng
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  // Hiệu ứng nhấp nháy cho đèn được chọn
  if (activeLed != -1) {
    // Nhấp nháy 3 lần
    for(int blink = 0; blink < 3; blink++) {
      digitalWrite(activeLed, HIGH); // Bật sáng
      delay(200);                    // Giữ sáng
      digitalWrite(activeLed, LOW);  // Tắt
      delay(200);                    // Giữ tắt
    }
    // Giữ sáng cuối cùng
    digitalWrite(activeLed, HIGH);
    delay(1000);
    digitalWrite(activeLed, LOW);
  }
}