#include <Arduino.h>

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-esp32-motion-detection-arduino/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Initialize Telegram BOT
#define BOTtoken "xxxxx"  // Thay Bot Token của bạn vào đây
#define GROUP_ID "group_chatid" // Thay Group ID của bạn vào đây (nhớ giữ lại dấu trừ nếu có)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int motionSensor = 27; // PIR Motion Sensor
const int ledPin = 23;       // Red LED
bool motionDetected = false;

// Các biến dùng để thiết lập thời gian lấy tin nhắn mới từ Telegram
int botRequestDelay = 1000; // Kiểm tra tin nhắn mới mỗi 1 giây
unsigned long lastTimeBotRan = 0;

// Định dạng chuỗi %s,%d,...
String StringFormat(const char* fmt, ...){
  va_list vaArgs;
  va_start(vaArgs, fmt);
  va_list vaArgsCopy;
  va_copy(vaArgsCopy, vaArgs);
  const int iLen = vsnprintf(NULL, 0, fmt, vaArgsCopy);
  va_end(vaArgsCopy);
  int iSize = iLen + 1;
  char* buff = (char*)malloc(iSize);
  vsnprintf(buff, iSize, fmt, vaArgs);
  va_end(vaArgs);
  String s = buff;
  free(buff);
  return String(s);
}

// Indicates when motion is detected
void IRAM_ATTR detectsMovement() {
  motionDetected = true;
}

// Hàm xử lý tin nhắn nhận được từ Telegram
void handleNewMessages(int numNewMessages) {
  Serial.print("Có tin nhắn mới: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    // Lấy ID của group/người gửi và nội dung tin nhắn
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    // Chỉ phản hồi nếu tin nhắn đến từ đúng GROUP_ID của bạn (để bảo mật)
    if (chat_id != GROUP_ID) {
      continue;
    }

    // Xử lý lệnh /start
    if (text == "/start") {
      String welcome = "Xin chào, Dũng.\n";
      welcome += "Sử dụng các lệnh sau để điều khiển đèn LED.\n\n";
      welcome += "Gửi /led_on bật sáng đèn\n";
      welcome += "Gửi /led_off để tắt đèn\n";
      welcome += "Gửi /get_state để yêu cầu trạng thái đèn hiện tại";
      bot.sendMessage(chat_id, welcome, "");
    }

    // Xử lý lệnh /led_on
    if (text == "/led_on") {
      digitalWrite(ledPin, HIGH); // Bật LED vật lý
      bot.sendMessage(chat_id, "LED bật sáng", "");
    }

    // Xử lý lệnh /led_off
    if (text == "/led_off") {
      digitalWrite(ledPin, LOW); // Tắt LED vật lý
      bot.sendMessage(chat_id, "LED is OFF", "");
    }

    // Xử lý lệnh /get_state
    if (text == "/get_state") {
      // Đọc trạng thái chân Digital hiện tại
      if (digitalRead(ledPin) == HIGH) {
        bot.sendMessage(chat_id, "LED is ON", "");
      } else {
        bot.sendMessage(chat_id, "LED is OFF", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Cài đặt chân ngắt cho cảm biến PIR
  pinMode(motionSensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  // Cài đặt chân LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Kết nối WiFi
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nWiFi connected");
  bot.sendMessage(GROUP_ID, "IoT Developer started up", "");
}

void loop() {
  static uint count_ = 0;

  // 1. TÍNH NĂNG CẢNH BÁO CHUYỂN ĐỘNG
  if(motionDetected){
    ++count_;
    Serial.print(count_);Serial.println(". MOTION DETECTED => Waiting to send to Telegram");    
    
    // Gửi cảnh báo lên Telegram
    String msg = StringFormat("%u => Motion detected!", count_);
    bot.sendMessage(GROUP_ID, msg.c_str(), "");
    Serial.print(count_);Serial.println(". Sent successfully to Telegram");
    
    motionDetected = false;
  }

  // 2. TÍNH NĂNG LẮNG NGHE LỆNH ĐIỀU KHIỂN LED
  // Kiểm tra tin nhắn mới sau mỗi khoảng thời gian (botRequestDelay)
  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    
    while (numNewMessages) {
      Serial.println("Đang xử lý tin nhắn...");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis(); // Cập nhật lại mốc thời gian
  }
}