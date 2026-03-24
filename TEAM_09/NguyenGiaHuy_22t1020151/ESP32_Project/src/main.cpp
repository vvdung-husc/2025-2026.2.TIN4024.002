/**
 * ============================================================================
 * DỰ ÁN: HỆ THỐNG GIÁM SÁT ĐA CẢM BIẾN (NCT_KK46)
 * ============================================================================
 * Phần cứng:
 *  - ESP8266 NodeMCU v2/v3
 *  - OLED SH1106 128x64 (I2C: SDA=D2, SCL=D1)
 *  - DHT22 (Data=D3)
 *  - PIR HC-SR501 (Out=D5)
 *  - MQ-2 Gas (Aout=A0)
 *  - LED tích hợp (D4)
 * 
 * Cập nhật mới nhất:
 *  - Xử lý PIR không làm chiếm dụng màn hình quá lâu (Cooldown 15s).
 *  - Hiển thị cảnh báo Gas ưu tiên cao nhất.
 *  - Hiển thị thông số môi trường liên tục.
 * ============================================================================
 */

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// ============================================================================
// 1. CẤU HÌNH CHÂN CẮM (PIN CONFIG)
// ============================================================================
#define DHT_PIN       D3      // Chân dữ liệu DHT22
#define DHT_TYPE      DHT22   // Loại cảm biến
#define LED_PIN       D4      // LED tích hợp (LOW = Sáng)
#define PIR_PIN       D5      // Chân tín hiệu PIR
#define MQ2_PIN       A0      // Chân Analog đọc khí gas

// ============================================================================
// 2. CẤU HÌNH NGƯỠNG & THỜI GIAN (CONFIG)
// ============================================================================
// Ngưỡng khí gas (0-1023). Cần hiệu chỉnh theo thực tế môi trường.
// Giá trị tham khảo: Không khí sạch ~100-300. Có khói/gas ~400+
#define GAS_THRESHOLD     400   

// Thời gian hiển thị cảnh báo (ms)
#define PIR_ALERT_TIME    2000   // Chỉ hiện màn hình cảnh báo PIR trong 2 giây
#define PIR_COOLDOWN_TIME 15000  // Sau đó nghỉ 15 giây không báo nữa (tránh spam)
#define GAS_ALERT_TIME    8000   // Cảnh báo gas trong 8 giây

// Thời gian đọc cảm biến
#define DHT_INTERVAL      2000   // DHT22 cần ít nhất 2s giữa các lần đọc

// ============================================================================
// 3. KHỞI TẠO ĐỐI TƯỢNG (OBJECTS)
// ============================================================================
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire);

// ============================================================================
// 4. BIẾN TOÀN CỤC (GLOBAL VARIABLES)
// ============================================================================
float temperature = 0.0;
float humidity    = 0.0;
int   gasValue    = 0;

// Trạng thái PIR
bool pirTriggered   = false;   // Đang có chuyển động mới
bool pirInCooldown  = false;   // Đang trong thời gian nghỉ
unsigned long pirAlertStart   = 0;
unsigned long pirCooldownStart = 0;

// Trạng thái GAS
bool gasAlert       = false;
unsigned long gasAlertStart   = 0;

// Thời gian đọc DHT
unsigned long lastDHTRead = 0;

// ============================================================================
// 5. CÁC HÀM HỖ TRỢ (HELPER FUNCTIONS)
// ============================================================================

// --- Đọc khí gas ổn định (Lấy trung bình 5 mẫu) ---
int readGasSensor() {
  long sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += analogRead(MQ2_PIN);
    delay(10);
  }
  return sum / 5;
}

// --- Hiển thị màn hình chính (Nhiệt độ, Độ ẩm, Gas, Icon PIR) ---
void displayMainScreen() {
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  
  // 1. Tiêu đề
  display.setTextSize(1);
  display.setCursor(20, 0);
  display.println("NCT_KK46");
  
  // 2. Icon trạng thái PIR ở góc phải (Nhấp nháy nếu đang cooldown)
  if (pirInCooldown) {
    // Đang trong thời gian nghỉ (vẫn theo dõi nhưng không báo động)
    if ((millis() / 500) % 2 == 0) {
      display.fillRect(118, 0, 10, 10, SH110X_WHITE);
      display.setTextColor(SH110X_BLACK);
    } else {
      display.drawRect(118, 0, 10, 10, SH110X_WHITE);
      display.setTextColor(SH110X_WHITE);
    }
    display.setCursor(120, 1);
    display.print("P");
  } else {
    // Bình thường
    display.drawRect(118, 0, 10, 10, SH110X_WHITE);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(120, 1);
    display.print("P");
  }
  display.setTextColor(SH110X_WHITE); // Reset màu
  
  display.drawLine(0, 10, 127, 10, SH110X_WHITE);
  
  // 3. Hàng thông số 1 (Nhiệt & Ẩm)
  display.setCursor(0, 14);
  display.print("T:");
  display.setTextSize(1);
  display.printf("%.1fC", temperature);
  
  display.setCursor(55, 14);
  display.print("H:");
  display.printf("%.1f%%", humidity);
  
  // 4. Hàng thông số 2 (Khí Gas & Thanh tiến trình)
  display.setCursor(0, 28);
  display.print("G:");
  display.print(gasValue);
  
  // Vẽ thanh tiến trình gas
  int barWidth = map(constrain(gasValue, 0, 1023), 0, 1023, 0, 80);
  display.drawRect(25, 30, 80, 5, SH110X_WHITE);
  if (barWidth > 0) {
    // Nếu gas cao thì tô đậm hơn
    if (gasValue > GAS_THRESHOLD) {
      display.fillRect(26, 31, barWidth - 1, 3, SH110X_WHITE);
    } else {
      // Kẻ vạch chéo cho mức bình thường
      for (int i = 26; i < 26 + barWidth; i += 4) {
        display.drawLine(i, 31, i + 2, 33, SH110X_WHITE);
      }
    }
  }
  
  // 5. Đường phân cách dưới
  display.drawLine(0, 40, 127, 40, SH110X_WHITE);
  
  // 6. Trạng thái hệ thống
  display.setCursor(0, 45);
  display.print("PIR:");
  display.print(pirInCooldown ? "WAIT" : "READY");
  
  display.setCursor(60, 45);
  display.print("GAS:");
  display.print(gasAlert ? "ALERT" : "OK");
  
  // 7. Footer
  display.drawLine(0, 55, 127, 55, SH110X_WHITE);
  display.setCursor(30, 58);
  display.setTextSize(1);
  display.print("Monitoring System");
  
  display.display();
}

// --- Hiển thị cảnh báo PIR (Toàn màn hình trong thời gian ngắn) ---
void displayPIRAlert() {
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  
  display.setTextSize(1);
  display.setCursor(25, 0);
  display.println("NCT_KK46");
  display.drawLine(0, 10, 127, 10, SH110X_WHITE);
  
  display.setTextSize(2);
  display.setCursor(15, 25);
  display.println("MOTION!");
  display.setCursor(35, 45);
  display.println("DETECTED");
  
  // Vẽ icon người chạy đơn giản
  display.drawLine(110, 20, 115, 30, SH110X_WHITE);
  display.drawLine(115, 30, 108, 38, SH110X_WHITE);
  display.drawLine(115, 30, 122, 38, SH110X_WHITE);
  display.drawLine(110, 20, 120, 20, SH110X_WHITE);
  
  display.display();
}

// --- Hiển thị cảnh báo GAS (Ưu tiên cao nhất) ---
void displayGasAlert() {
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  
  display.setTextSize(1);
  display.setCursor(25, 0);
  display.println("NCT_KK46");
  display.drawLine(0, 10, 127, 10, SH110X_WHITE);
  
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("CANH BAO");
  display.setCursor(25, 40);
  display.println("KHI GAS!");
  
  display.setTextSize(1);
  display.setCursor(10, 55);
  display.print("Level: ");
  display.print(gasValue);
  
  // Vẽ icon lửa
  display.drawLine(115, 25, 120, 35, SH110X_WHITE);
  display.drawLine(120, 35, 125, 25, SH110X_WHITE);
  display.drawLine(115, 25, 125, 25, SH110X_WHITE);
  
  display.display();
}

// ============================================================================
// 6. SETUP & LOOP
// ============================================================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== KHOI DONG HE THONG ===");
  
  // Cấu hình chân
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  digitalWrite(LED_PIN, HIGH); // LED tắt (HIGH) khi bắt đầu
  
  // Khởi tạo cảm biến
  dht.begin();
  
  // Khởi tạo I2C & OLED
  Wire.begin(D2, D1); // SDA, SCL
  if (!display.begin(0x3C, true)) {
    Serial.println("Loi: Khong tim thay OLED!");
    while (1) {
      digitalWrite(LED_PIN, LOW); delay(500); digitalWrite(LED_PIN, HIGH); delay(500);
    }
  }
  
  // Màn hình chào
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(15, 10); display.println("DA KHOI DONG");
  display.setCursor(25, 25); display.println("NCT_KK46");
  display.setCursor(10, 40); display.println("Multi-Sensor");
  display.setCursor(20, 54); display.println("Monitoring");
  display.display();
  
  delay(2000);
  
  Serial.println("He thong san sang. MQ-2 dang preheat...");
}

void loop() {
  unsigned long now = millis();
  
  // ==========================================================================
  // 1. XỬ LÝ PIR (CHỐNG NHẢY & COOLDOWN)
  // ==========================================================================
  int pirState = digitalRead(PIR_PIN);
  
  // Nếu có tín hiệu HIGH và không đang trong thời gian nghỉ (cooldown)
  if (pirState == HIGH && !pirInCooldown && !pirTriggered) {
    pirTriggered = true;
    pirAlertStart = now;
    pirInCooldown = false;
    Serial.println(">>> [PIR] Phat hien chuyen dong!");
    
    // Nhấp nháy LED báo động nhanh
    for(int i=0; i<3; i++) {
      digitalWrite(LED_PIN, LOW); delay(50);
      digitalWrite(LED_PIN, HIGH); delay(50);
    }
  }
  
  // Kiểm tra thời gian hiển thị cảnh báo PIR (2 giây)
  if (pirTriggered) {
    if (now - pirAlertStart >= PIR_ALERT_TIME) {
      pirTriggered = false;       // Tắt trạng thái báo động full màn hình
      pirInCooldown = true;       // Bật chế độ nghỉ
      pirCooldownStart = now;     // Bắt đầu đếm thời gian nghỉ
      Serial.println("<<< [PIR] Het thoi gian bao, vao che do nghi.");
    }
  }
  
  // Kiểm tra thời gian nghỉ (Cooldown 15 giây)
  if (pirInCooldown) {
    if (now - pirCooldownStart >= PIR_COOLDOWN_TIME) {
      pirInCooldown = false;      // Hết nghỉ, sẵn sàng phát hiện lại
      Serial.println("<<< [PIR] Het thoi gian nghi, san sang.");
    }
  }
  
  // ==========================================================================
  // 2. XỬ LÝ GAS (MQ-2)
  // ==========================================================================
  gasValue = readGasSensor();
  
  if (gasValue >= GAS_THRESHOLD && !gasAlert) {
    gasAlert = true;
    gasAlertStart = now;
    Serial.printf(">>> [GAS] Canh bao! Level: %d\n", gasValue);
  }
  
  // Tự tắt cảnh báo gas sau thời gian quy định nếu mức khí giảm
  if (gasAlert) {
    if (now - gasAlertStart >= GAS_ALERT_TIME) {
      if (gasValue < (GAS_THRESHOLD * 0.8)) { // Chỉ tắt nếu khí đã giảm xuống dưới 80% ngưỡng
        gasAlert = false;
        Serial.println("<<< [GAS] Da an toan.");
      } else {
        gasAlertStart = now; // Reset thời gian nếu khí vẫn cao
      }
    }
  }
  
  // ==========================================================================
  // 3. ĐỌC DHT22 (2 GIÂY/LẦN)
  // ==========================================================================
  if (now - lastDHTRead >= DHT_INTERVAL) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    
    if (!isnan(temp) && !isnan(hum)) {
      temperature = temp;
      humidity = hum;
      // Serial.printf("[DHT] T: %.1f | H: %.1f\n", temp, hum);
    }
    lastDHTRead = now;
  }
  
  // ==========================================================================
  // 4. HIỂN THỊ OLED (ƯU TIÊN: GAS > PIR > MAIN)
  // ==========================================================================
  
  if (gasAlert) {
    // Ưu tiên 1: Cảnh báo Gas (Nguy hiểm nhất)
    displayGasAlert();
    digitalWrite(LED_PIN, LOW); // LED sáng liên tục
    delay(100); 
    return; // Thoát vòng loop để ưu tiên xử lý alert
  }
  
  if (pirTriggered) {
    // Ưu tiên 2: Cảnh báo PIR (Chỉ 2 giây)
    displayPIRAlert();
    digitalWrite(LED_PIN, LOW);
    delay(100);
    return;
  }
  
  // Bình thường: Hiển thị màn hình chính
  displayMainScreen();
  
  // LED heartbeat (Nhấp nháy chậm báo hiệu hệ thống sống)
  static bool ledState = false;
  if (now % 1000 < 500) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? LOW : HIGH);
  }
  
  delay(200); // Delay nhỏ để ổn định vòng lặp
}