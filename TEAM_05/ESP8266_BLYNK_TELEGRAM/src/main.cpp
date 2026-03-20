/*
  THÔNG TIN NHÓM 05
  
  1. Nguyễn Trung

*/

#include <Arduino.h>
#include <WiFi.h>              // Thay thế cho ESP8266WiFi
#include <WiFiClientSecure.h>

// --- BLYNK CONFIG ---
#define BLYNK_TEMPLATE_ID "TMPL6aF7y2BZF"
#define BLYNK_TEMPLATE_NAME "BlynkTelegram"
#define BLYNK_AUTH_TOKEN "9bpUUTTjZX9jSwDwrdV-uiVLRysOef_e"
#include <BlynkSimpleEsp32.h>  // Thay thế cho BlynkSimpleEsp8266

// --- TELEGRAM CONFIG ---
#include <UniversalTelegramBot.h>
#define BOTtoken "8671035194:AAEwQRhO6M-Zh5yZZAwweqfXiaOfYAfqNPI"
#define GROUP_ID "-1003845282753" 

// --- SENSOR CONFIG ---
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>

#define DHTPIN 15      // Chân D15 (GPIO15) cho ESP32
#define DHTTYPE DHT22  
#define MQ2PIN 34      // Chân ADC (GPIO34) cho ESP32
#define LED_PIN 2      // Chân D2 (LED Built-in của ESP32)

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Khởi tạo các đối tượng
DHT dht(DHTPIN, DHTTYPE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Các biến lưu trữ
float temp = 0.0, hum = 0.0;
float lastTemp = 0.0, lastHum = 0.0;
int gasValue = 0;
bool ledState = false; 
unsigned long currentMillis;
unsigned long lastSensorUpdate = 0;
unsigned long lastTelegramCheck = 0;

// --- HÀM XỬ LÝ LỆNH TỪ TELEGRAM ---
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    if (chat_id != GROUP_ID) continue;

    if (text == "/led_on") {
      ledState = true;
      digitalWrite(LED_PIN, HIGH); // ESP32 LED Built-in sáng ở mức HIGH
      bot.sendMessage(chat_id, "💡 Đèn LED đã được BẬT", "");
      Blynk.virtualWrite(V5, 1);  
    }
    else if (text == "/led_off") {
      ledState = false;
      digitalWrite(LED_PIN, LOW); // Tắt ở mức LOW
      bot.sendMessage(chat_id, "🌑 Đèn LED đã được TẮT", "");
      Blynk.virtualWrite(V5, 0); 
    }
    else if (text == "/led_status") {
      String status = ledState ? "ĐANG BẬT" : "ĐANG TẮT";
      bot.sendMessage(chat_id, "Trạng thái LED hiện tại: " + status, "");
    }
    else if (text == "/get_weather") {
      String msg = "🌡 Thông tin môi trường hiện tại:\n";
      msg += "- Nhiệt độ: " + String(temp, 1) + " °C\n";
      msg += "- Độ ẩm: " + String(hum, 1) + " %\n";
      msg += "- Khí GAS: " + String(gasValue);
      
      // Thêm đánh giá an toàn cho ngầu
      if (gasValue > 2000) {
        msg += " ⚠️ (CẢNH BÁO CAO!)";
      } else {
        msg += " ✅ (An toàn)";
      }
      
      bot.sendMessage(chat_id, msg, "");
    }
  }
}

// --- ĐỒNG BỘ NÚT BẤM TỪ BLYNK ---
BLYNK_WRITE(V5) {
  int pinValue = param.asInt();
  ledState = (pinValue == 1);
  digitalWrite(LED_PIN, ledState ? HIGH : LOW); // ESP32 Logic
}

void setup() {
  Serial.begin(115200);
  
  // Cài đặt chân
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Tắt LED ban đầu (ESP32)
  
  dht.begin();
  u8g2.begin();

  client.setInsecure();

  // Kết nối WiFi và Blynk
  Serial.print("Connecting to WiFi...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  Serial.println("Connected!");

  bot.sendMessage(GROUP_ID, "✅ Hệ thống IoT - Team 05 đã khởi động!", "");
}

void loop() {
  Blynk.run();
  currentMillis = millis();

  // 1. ĐỌC CẢM BIẾN, CẬP NHẬT MỖI 2 GIÂY
  if (currentMillis - lastSensorUpdate >= 2000) {
    lastSensorUpdate = currentMillis;

    hum = dht.readHumidity();
    temp = dht.readTemperature();
    gasValue = analogRead(MQ2PIN);

    if (isnan(hum) || isnan(temp)) return;

    // --- HIỂN THỊ OLED ---
    u8g2.clearBuffer();          
    u8g2.setFont(u8g2_font_ncenB08_tr); 
    
    u8g2.setCursor(0, 12); u8g2.print("Nhiet do: "); u8g2.print(temp, 1); u8g2.print(" C");
    u8g2.setCursor(0, 26); u8g2.print("Do am: "); u8g2.print(hum, 1); u8g2.print(" %");
    u8g2.setCursor(0, 40); u8g2.print("Khi GAS: "); u8g2.print(gasValue);
    
    const char* teamName = "- TEAM 05 -";
    int textX = (128 - u8g2.getStrWidth(teamName)) / 2;
    u8g2.setCursor(textX, 60); 
    u8g2.print(teamName);
    u8g2.sendBuffer(); 

    // --- GỬI LÊN BLYNK ---
    Blynk.virtualWrite(V0, "Team 05 - IOT"); 
    Blynk.virtualWrite(V1, temp);
    Blynk.virtualWrite(V2, (int)hum); 
    Blynk.virtualWrite(V3, currentMillis / 1000); 
    Blynk.virtualWrite(V4, gasValue);

    // --- CẢNH BÁO TELEGRAM ---
    if (abs(temp - lastTemp) >= 1.0 || abs(hum - lastHum) >= 5.0) {
      String alertMsg = "⚠️ Cập nhật cảm biến!\n";
      alertMsg += "- Nhiệt độ mới: " + String(temp, 1) + " °C\n";
      alertMsg += "- Độ ẩm mới: " + String(hum, 1) + " %";
      bot.sendMessage(GROUP_ID, alertMsg, "");
      
      lastTemp = temp;
      lastHum = hum;
    }
  }

  // 2. KIỂM TRA TIN NHẮN TELEGRAM MỖI 1 GIÂY
  if (currentMillis - lastTelegramCheck > 1000) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTelegramCheck = currentMillis;
  }
}