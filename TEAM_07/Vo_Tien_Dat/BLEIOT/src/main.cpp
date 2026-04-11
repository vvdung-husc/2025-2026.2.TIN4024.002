// ================= COPY 3 DÒNG TỪ BLYNK VÀO ĐÂY =================
#define BLYNK_TEMPLATE_ID "TMPL6od6efg5C"
#define BLYNK_TEMPLATE_NAME "IPS ESP32"
#define BLYNK_AUTH_TOKEN "z-zKIg-ZUqtgCmjlaW5n5dbeTD3ek8o7"
// ================================================================

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <math.h>

// ================= THÔNG SỐ MẠNG =================
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// ================= CẤU HÌNH PHẦN CỨNG =================
const int PIN_D1 = 32; 
const int PIN_D2 = 33; 
const int PIN_D3 = 34;

// Tọa độ 3 trạm Anchor cố định (phòng 10x10)
const float x_1 = 0.0;  const float y_1 = 0.0;
const float x_2 = 10.0; const float y_2 = 0.0;
const float x_3 = 0.0;  const float y_3 = 10.0;
const float MAX_DISTANCE = 15.0; 

BlynkTimer timer; // Bộ đếm thời gian của Blynk

// Hàm giải hệ phương trình (Trilateration)
void calculateTrilateration(float r1, float r2, float r3, float &x, float &y) {
  float A = 2 * (x_2 - x_1);
  float B = 2 * (y_2 - y_1);
  float C = pow(r1, 2) - pow(r2, 2) - pow(x_1, 2) + pow(x_2, 2) - pow(y_1, 2) + pow(y_2, 2);
  
  float D = 2 * (x_3 - x_1);
  float E = 2 * (y_3 - y_1);
  float F = pow(r1, 2) - pow(r3, 2) - pow(x_1, 2) + pow(x_3, 2) - pow(y_1, 2) + pow(y_3, 2);

  float denominator = (A * E) - (B * D);
  
  if (denominator != 0) {
    x = ((C * E) - (B * F)) / denominator;
    y = ((A * F) - (C * D)) / denominator;
  } else {
    x = 0; y = 0;
  }
}

// Hàm đọc cảm biến, tính toán và gửi lên Blynk
void processAndSendData() {
  // 1. Đọc ADC giả lập khoảng cách
  float d1 = (analogRead(PIN_D1) / 4095.0) * MAX_DISTANCE;
  float d2 = (analogRead(PIN_D2) / 4095.0) * MAX_DISTANCE;
  float d3 = (analogRead(PIN_D3) / 4095.0) * MAX_DISTANCE;

  // 2. Tính tọa độ
  float target_x = 0.0;
  float target_y = 0.0;
  calculateTrilateration(d1, d2, d3, target_x, target_y);

  // In ra Serial Monitor
  Serial.print("D1:"); Serial.print(d1); 
  Serial.print(" | D2:"); Serial.print(d2); 
  Serial.print(" | D3:"); Serial.print(d3); 
  Serial.print(" ===> X:"); Serial.print(target_x); 
  Serial.print(" Y:"); Serial.println(target_y);

  // 3. Gửi dữ liệu lên Blynk (Các chân Virtual V1 -> V5)
  Blynk.virtualWrite(V1, target_x);
  Blynk.virtualWrite(V2, target_y);
  Blynk.virtualWrite(V3, d1);
  Blynk.virtualWrite(V4, d2);
  Blynk.virtualWrite(V5, d3);
}

void setup() {
  delay(2000); // Chờ 2 giây để Terminal kịp bật lên
  Serial.begin(115200);
  Serial.println("\n--- BAT DAU KHOI DONG ---");
  pinMode(PIN_D1, INPUT);
  pinMode(PIN_D2, INPUT);
  pinMode(PIN_D3, INPUT);

  // Kết nối Wi-Fi và Blynk Server
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Cài đặt chu kỳ gửi dữ liệu là 1 giây/lần (1000ms)
  timer.setInterval(1000L, processAndSendData);
}

void loop() {
  Blynk.run(); // Duy trì kết nối Blynk
  timer.run(); // Chạy bộ đếm thời gian
}

