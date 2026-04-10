#define BLYNK_TEMPLATE_ID "TMPL6ZLtsjXAW"
#define BLYNK_TEMPLATE_NAME "LEDESP32"
#define BLYNK_AUTH_TOKEN "UkYuHXKyGzG-zf2lHKlUNQw3-rjAX3hZ"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_NeoPixel.h>

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_NeoPixel.h>

// Cấu hình chân Pin
#define PIN_LED 15
#define NUM_LEDS 16 // Điều chỉnh lại cho đúng số bóng trong vòng LED của bạn

Adafruit_NeoPixel strip(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Biến lưu trữ trạng thái
int ledPower = 1;     // Mặc định cho là 1 để mới vào đèn sáng luôn
int rVal = 255;       
int gVal = 255;       
int bVal = 255;       
int brightness = 150; 

// Hàm cập nhật màu sắc LED tập trung
void updateLEDs() {
  if (ledPower == 1) {
    strip.setBrightness(brightness);
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(rVal, gVal, bVal));
    }
  } else {
    strip.clear(); // Tắt toàn bộ bóng nếu V0 = 0
  }
  strip.show();
}

// Khi Blynk kết nối thành công, đồng bộ dữ liệu từ Server xuống
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0, V1, V2, V3, V4);
}

// Bật/Tắt hệ thống (V0)
BLYNK_WRITE(V0) {
  ledPower = param.asInt();
  updateLEDs();
}

// Điều chỉnh màu Đỏ (V1)
BLYNK_WRITE(V1) {
  rVal = param.asInt();
  updateLEDs();
}

// Điều chỉnh màu Xanh Lá (V2)
BLYNK_WRITE(V2) {
  gVal = param.asInt();
  updateLEDs();
}

// Điều chỉnh màu Xanh Dương (V3)
BLYNK_WRITE(V3) {
  bVal = param.asInt();
  updateLEDs();
}

// Điều chỉnh Độ sáng (V4)
BLYNK_WRITE(V4) {
  brightness = param.asInt();
  updateLEDs();
}

// Nút bấm phụ (V5) - Random màu
BLYNK_WRITE(V5) {
  if (param.asInt() == 1) {
    rVal = random(0, 256);
    gVal = random(0, 256);
    bVal = random(0, 256);
    // Gửi giá trị mới ngược lại App để đồng bộ các thanh Slider
    Blynk.virtualWrite(V1, rVal);
    Blynk.virtualWrite(V2, gVal);
    Blynk.virtualWrite(V3, bVal);
    updateLEDs();
  }
}

void setup() {
  Serial.begin(115200);
  
  strip.begin();
  strip.show(); // Khởi tạo trạng thái tắt
  
  Serial.println("Dang ket noi Blynk...");
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
}