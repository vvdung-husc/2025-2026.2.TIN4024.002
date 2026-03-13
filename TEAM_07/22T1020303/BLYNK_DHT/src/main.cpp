// 1. Khai báo thông tin kết nối Blynk (Lấy từ Device Info)
#define BLYNK_TEMPLATE_ID "TMPL665IAoir9"
#define BLYNK_TEMPLATE_NAME "ESMART"
#define BLYNK_AUTH_TOKEN "kaqJuRl7t289QOjK6kN_b9ptU2_0Hp14"

// 2. Khai báo các thư viện cần thiết
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHTesp.h"

// 3. Cấu hình chân Pin và biến điều khiển
const int DHT_PIN = 15; // Chân Data DHT22 nối vào GPIO 15
const int LED_PIN = 2;  // Đèn LED nối vào GPIO 2 (tương ứng RELAY1)
DHTesp dhtSensor;
BlynkTimer timer;

// Thông tin mạng WiFi (Dùng cho Wokwi Simulator)
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST"; 
char pass[] = "";

// 4. Hàm đọc cảm biến và gửi lên Blynk (V0, V1)
void sendSensorData() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  
  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("Lỗi: Không đọc được cảm biến DHT22!");
    return;
  }

  // Gửi lên các Virtual Pin đã thiết lập trên Blynk Dashboard
  Blynk.virtualWrite(V0, data.temperature); // Gauge Nhiệt độ
  Blynk.virtualWrite(V1, data.humidity);    // Gauge Độ ẩm
  
  Serial.print("Nhiet do: "); Serial.print(data.temperature);
  Serial.print("C | Do am: "); Serial.print(data.humidity); Serial.println("%");
}

// 5. Hàm nhận lệnh từ nút nhấn trên Blynk (V2)
BLYNK_WRITE(V2) { // V2 khớp với Datastream RELAY1 bạn đã tạo
  int value = param.asInt(); 
  digitalWrite(LED_PIN, value); 
  Serial.print("Trạng thái đèn: "); Serial.println(value == 1 ? "BẬT" : "TẮT");
}

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo linh kiện
  pinMode(LED_PIN, OUTPUT);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  
  // Kết nối tới Blynk Server
  Blynk.begin(auth, ssid, pass);
  
  // Thiết lập gửi dữ liệu cảm biến định kỳ mỗi 2 giây
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}