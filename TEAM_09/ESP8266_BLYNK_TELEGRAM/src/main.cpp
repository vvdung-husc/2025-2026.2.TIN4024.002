/*
  THÔNG TIN NHÓM 09
  1. Nguyễn Đăng Khôi
  2. Nguyễn Gia Huy
  3. Nguyễn Lê Bá Minh  

  */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// --- BLYNK CONFIG ---
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""
#include <BlynkSimpleEsp8266.h>

// --- TELEGRAM CONFIG ---
#include <UniversalTelegramBot.h>
#define BOTtoken ""
#define GROUP_ID ""

// --- SENSOR CONFIG ---
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>

#define DHTPIN 0       // Chân D3
#define DHTTYPE DHT22  // Loại cảm biến DHT22
#define MQ2PIN A0      // Chân Analog A0
#define LED_PIN 2      // Chân D4 (LED Built-in)

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Khởi tạo các đối tượng
DHT dht(DHTPIN, DHTTYPE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Các biến lưu trữ
float temp = 0.0, hum = 0.0;
float lastTemp = 0.0, lastHum = 0.0;
int gasValue = 0;
bool ledState = false; // Trạng thái LED (false = tắt)
unsigned long currentMillis;
unsigned long lastSensorUpdate = 0;
unsigned long lastTelegramCheck = 0;

// --- HÀM XỬ LÝ LỆNH TỪ TELEGRAM ---
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    // Chỉ nhận lệnh từ đúng nhóm
    if (chat_id != GROUP_ID) continue;

    if (text == "/led_on") {
      ledState = true;
      digitalWrite(LED_PIN, LOW); // ESP8266 LED Built-in sáng ở mức LOW
      bot.sendMessage(chat_id, "💡 Đèn LED đã được BẬT", "");
      Blynk.virtualWrite(V5, 1);  // Đồng bộ trạng thái lên Switch của Blynk
    }
    else if (text == "/led_off") {
      ledState = false;
      digitalWrite(LED_PIN, HIGH);
      bot.sendMessage(chat_id, "🌑 Đèn LED đã được TẮT", "");
      Blynk.virtualWrite(V5, 0); 
    }
    else if (text == "/led_status") {
      String status = ledState ? "ĐANG BẬT" : "ĐANG TẮT";
      bot.sendMessage(chat_id, "Trạng thái LED hiện tại: " + status, "");
    }
    else if (text == "/get_weather") {
      String msg = "🌡 Thời tiết hiện tại:\n";
      msg += "- Nhiệt độ: " + String(temp, 1) + " °C\n";
      msg += "- Độ ẩm: " + String(hum, 1) + " %";
      bot.sendMessage(chat_id, msg, "");
    }
  }
}

// --- ĐỒNG BỘ NÚT BẤM (SWITCH) TỪ BLYNK VỀ MẠCH ---
BLYNK_WRITE(V5) {
  int pinValue = param.asInt();
  ledState = (pinValue == 1);
  digitalWrite(LED_PIN, ledState ? LOW : HIGH); // Đồng bộ LED vật lý
}

void setup() {
  Serial.begin(115200);
  
  // Cài đặt chân
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Tắt LED ban đầu
  
  dht.begin();
  u8g2.begin();

  // Bật chế độ không kiểm tra chứng chỉ cho Telegram trên ESP8266 (Bắt buộc)
  client.setInsecure();

  // Kết nối WiFi và Blynk
  Serial.print("Connecting to WiFi...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  Serial.println("Connected!");

  bot.sendMessage(GROUP_ID, "✅ Hệ thống IoT - Team 09 đã khởi động!", "");
}

void loop() {
  Blynk.run();
  currentMillis = millis();

  // 1. ĐỌC CẢM BIẾN, CẬP NHẬT OLED & BLYNK MỖI 2 GIÂY
  if (currentMillis - lastSensorUpdate >= 2000) {
    lastSensorUpdate = currentMillis;

    hum = dht.readHumidity();
    temp = dht.readTemperature();
    gasValue = analogRead(MQ2PIN);

    // Nếu đọc lỗi thì bỏ qua vòng này
    if (isnan(hum) || isnan(temp)) return;

    // --- HIỂN THỊ OLED ---
    u8g2.clearBuffer();          
    u8g2.setFont(u8g2_font_ncenB08_tr); 
    
    u8g2.setCursor(0, 12); u8g2.print("Nhiet do: "); u8g2.print(temp, 1); u8g2.print(" C");
    u8g2.setCursor(0, 26); u8g2.print("Do am: "); u8g2.print(hum, 1); u8g2.print(" %");
    u8g2.setCursor(0, 40); u8g2.print("Khi GAS: "); u8g2.print(gasValue);
    
    // In thông tin Team X căn giữa ở cuối
    const char* teamName = "- TEAM X -";
    int textX = (128 - u8g2.getStrWidth(teamName)) / 2;
    u8g2.setCursor(textX, 60); 
    u8g2.print(teamName);
    u8g2.sendBuffer(); 

    // --- GỬI LÊN BLYNK ---
    Blynk.virtualWrite(V0, "Team X - IOT"); 
    Blynk.virtualWrite(V1, temp);
    Blynk.virtualWrite(V2, (int)hum); // Ép kiểu về số nguyên vì bạn cấu hình V2 là Integer
    Blynk.virtualWrite(V3, currentMillis / 1000); // Uptime (giây)
    Blynk.virtualWrite(V4, gasValue);

    // --- CẢNH BÁO TELEGRAM NẾU NHIỆT ĐỘ/ĐỘ ẨM THAY ĐỔI LỚN ---
    // Gửi cảnh báo nếu nhiệt độ thay đổi > 1 độ hoặc độ ẩm thay đổi > 5%
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