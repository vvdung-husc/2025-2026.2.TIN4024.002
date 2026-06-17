/*
THÔNG TIN NHÓM 12
1. Lê Hữu Hoàng Nam - Mã SV: 21T1020524
2. Hoàng Phương Phi - Mã SV: 22T1020309
*/

// --- 1. THÔNG TIN BLYNK (PHẢI ĐỂ TRÊN CÙNG) ---
#define BLYNK_TEMPLATE_ID "TMPL6coTLv9fn"
#define BLYNK_TEMPLATE_NAME "ESP8266BLYNKTELEGRAM"
#define BLYNK_AUTH_TOKEN "iSUaFwZClOfGhii6OEpj18giQRF2YodO"

#define BLYNK_PRINT Serial // Cứu tinh giúp in lỗi Blynk ra Terminal!

// --- 2. THƯ VIỆN ---
#include <WiFi.h> 
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h> 
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "DHT.h"

// --- 3. WIFI & TELEGRAM ---
char ssid[] = "Wokwi-GUEST"; 
char pass[] = "";

#define BOT_TOKEN "8444181330:AAGKsAVJCcZUa7_9NgyFRDqEES3ekgVTqF8" 
#define CHAT_ID "-5185301154"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// --- 4. CẤU HÌNH CẢM BIẾN ---
#define DHTPIN 15      
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);

#define LED_PIN 2      
#define MQ2_PIN 34     

BlynkTimer timer;
float lastTemp = 0;
float lastHum = 0;
int ledStatus = 0;

// --- 5. XỬ LÝ LỆNH TỪ TELEGRAM ---
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    if (text == "/led_on") {
      digitalWrite(LED_PIN, HIGH); 
      ledStatus = 1;
      Blynk.virtualWrite(V1, 1);  
      bot.sendMessage(chat_id, "Đèn LED đã được BẬT 🟢", "");
    } 
    else if (text == "/led_off") {
      digitalWrite(LED_PIN, LOW);
      ledStatus = 0;
      Blynk.virtualWrite(V1, 0);  
      bot.sendMessage(chat_id, "Đèn LED đã được TẮT 🔴", "");
    } 
    else if (text == "/led_status") {
      if (ledStatus == 1) bot.sendMessage(chat_id, "Trạng thái: Đèn đang BẬT", "");
      else bot.sendMessage(chat_id, "Trạng thái: Đèn đang TẮT", "");
    } 
    else if (text == "/get_weather") {
      String msg = "🌡️ Nhiệt độ: " + String(lastTemp) + " °C\n";
      msg += "💧 Độ ẩm: " + String(lastHum) + " %";
      bot.sendMessage(chat_id, msg, "");
    }
  }
}

void checkTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

// --- 6. ĐỒNG BỘ NÚT TỪ BLYNK VỀ MẠCH ---
BLYNK_WRITE(V1) {
  ledStatus = param.asInt();
  if (ledStatus == 1) {
    digitalWrite(LED_PIN, HIGH);
    bot.sendMessage(CHAT_ID, "Blynk: Đèn đã BẬT", "");
  } else {
    digitalWrite(LED_PIN, LOW);
    bot.sendMessage(CHAT_ID, "Blynk: Đèn đã TẮT", "");
  }
}

// --- 7. ĐỌC CẢM BIẾN VÀ GỬI LÊN BLYNK ---
void sendSensorData() {
  Blynk.virtualWrite(V0, millis() / 1000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    Blynk.virtualWrite(V2, t);
    Blynk.virtualWrite(V3, h);

    if (abs(t - lastTemp) >= 1.0 || abs(h - lastHum) >= 5.0) {
      String alertMsg = "⚠️ Thời tiết thay đổi!\nNhiệt độ: " + String(t) + "°C\nĐộ ẩm: " + String(h) + "%";
      bot.sendMessage(CHAT_ID, alertMsg, "");
      lastTemp = t;
      lastHum = h;
    }
  }

  int gasValue = random(200, 600);
  Blynk.virtualWrite(V4, gasValue);
  Blynk.virtualWrite(V5, "Team 12"); 
}

// --- SETUP & LOOP ---
void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Khởi động Hệ thống ---");
  
  // 1. Kết nối WiFi thủ công trước cho chắc chắn
  Serial.println("Đang kết nối WiFi Wokwi-GUEST...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Đã kết nối WiFi!");

  // Bỏ qua xác thực SSL cho Telegram
  client.setInsecure();

  // 2. Kết nối Blynk sau khi đã có mạng
  Serial.println("Đang kết nối vào server Blynk...");
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(); // Nếu lỗi token, Terminal sẽ báo ngay ở bước này

  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); 

  timer.setInterval(2000L, sendSensorData); 
  timer.setInterval(3000L, checkTelegram);  
  
  Serial.println("🚀 Setup thành công! Chạy Loop...");
}

void loop() {
  Blynk.run();
  timer.run();
}