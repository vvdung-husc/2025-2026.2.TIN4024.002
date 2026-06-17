#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --- BƯỚC 1: ĐIỀN THÔNG TIN TELEGRAM ---
#define BOT_TOKEN "8749705377:AAGu2D1z0KC8-HpseoLZWH_aTxrxmzh9zPg"
#define CHAT_ID "-5285343449"

// --- BƯỚC 2: CẤU HÌNH WIFI WOKWI ---
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

// --- BƯỚC 3: CẤU HÌNH CHÂN ĐÈN LED ---
#define LED_PIN 2 // Chân nối với đèn LED (thường trên ESP32 dùng chân số 2)

unsigned long lastTime = 0;
long last_update_id = 0; // Biến lưu ID tin nhắn để không đọc lại tin cũ

// Hàm dùng để gửi tin nhắn từ ESP32 lên nhóm Telegram
void sendMessage(String message) {
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(BOT_TOKEN) + "/sendMessage?chat_id=" + String(CHAT_ID) + "&text=" + message;
  http.begin(url);
  http.GET();
  http.end();
}

void setup() {
  Serial.begin(115200);
  
  // Cài đặt chân LED là đầu ra và tắt đèn lúc mới khởi động
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); 

  // Kết nối WiFi
  Serial.print("Connecting to WiFi ");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  
  // Báo cáo lên Telegram khi ESP32 đã sẵn sàng
  sendMessage("Bot đã sẵn sàng! Gõ /on để bật đèn, /off để tắt đèn.");
}

void loop() {
  // ESP32 sẽ kiểm tra tin nhắn mới mỗi 2 giây (2000 miligiây)
  if (millis() - lastTime > 2000) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      // Gọi API getUpdates của Telegram để lấy tin nhắn
      String url = "https://api.telegram.org/bot" + String(BOT_TOKEN) + "/getUpdates?offset=" + String(last_update_id);
      http.begin(url);
      
      int httpCode = http.GET();
      if (httpCode > 0) {
        String payload = http.getString();
        
        // Dùng ArduinoJson để giải mã dữ liệu lấy về
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error && doc["ok"] == true) {
          JsonArray result = doc["result"].as<JsonArray>();
          
          for (JsonObject msg : result) {
            // Cập nhật lại update_id để lần sau bỏ qua tin nhắn này
            last_update_id = msg["update_id"].as<long>() + 1; 
            
            String text = msg["message"]["text"].as<String>();
            String chatId = msg["message"]["chat"]["id"].as<String>();
            
            // Nếu tin nhắn đến từ đúng nhóm IoT - K45 thì mới xử lý
            if (chatId == String(CHAT_ID)) {
              Serial.println("Nhận được lệnh: " + text);
              
              if (text == "/on") {
                digitalWrite(LED_PIN, HIGH); // Bật đèn
                sendMessage("Đã BẬT đèn LED! 💡");
              } 
              else if (text == "/off") {
                digitalWrite(LED_PIN, LOW); // Tắt đèn
                sendMessage("Đã TẮT đèn LED! 🌑");
              }
            }
          }
        }
      }
      http.end();
    }
    lastTime = millis();
  }
}