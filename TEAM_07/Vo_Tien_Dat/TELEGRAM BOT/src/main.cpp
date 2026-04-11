#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Thông tin mạng WiFi (Mô phỏng trên Wokwi)
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Token lấy từ BotFather và CHAT_ID nhóm của bạn
#define BOTtoken "8726597868:AAGByy9_xeVBMu-lcmmf5IGU2BDK9AEgMmk" 
#define CHAT_ID "-5209519780" 

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Khai báo chân cắm (Đã cập nhật khớp với sơ đồ Wokwi của bạn)
const int ledPin = 23;       // Đèn LED cắm ở chân D23 (Góc trên cùng bên phải)
const int motionSensor = 27; // Cảm biến PIR cắm ở chân D27 (Chân số 11 từ trên xuống, bên trái)

bool ledState = false; 
int pirState = LOW; // Biến lưu trạng thái cảm biến chuyển động

// Thời gian kiểm tra tin nhắn mới (1 giây/lần)
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

// Hàm xử lý tin nhắn từ Telegram
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "bạn";

    if (text == "/start") {
      String welcome = "Xin chào, " + from_name + ".\n";
      welcome += "Hệ thống IoT đã sẵn sàng!\n\n";
      welcome += "Gửi /led_on : Bật đèn\n";
      welcome += "Gửi /led_off : Tắt đèn\n";
      welcome += "Gửi /get_state : Kiểm tra trạng thái đèn";
      bot.sendMessage(chat_id, welcome, "");
    }
    else if (text == "/led_on") {
      digitalWrite(ledPin, HIGH);
      ledState = true;
      bot.sendMessage(chat_id, "💡 Đèn LED đã được BẬT", "");
    }
    else if (text == "/led_off") {
      digitalWrite(ledPin, LOW);
      ledState = false;
      bot.sendMessage(chat_id, "🌑 Đèn LED đã TẮT", "");
    }
    else if (text == "/get_state") {
      if (ledState) {
        bot.sendMessage(chat_id, "Trạng thái hiện tại: Đèn đang BẬT 💡", "");
      } else {
        bot.sendMessage(chat_id, "Trạng thái hiện tại: Đèn đang TẮT 🌑", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Cài đặt chân Output / Input
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(motionSensor, INPUT);

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Cấp quyền bảo mật cho thư viện Telegram

  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nKết nối WiFi thành công!");
  
  // Gửi tin nhắn thông báo vào nhóm khi mạch vừa khởi động xong
  bot.sendMessage(CHAT_ID, "✅ Hệ thống ESP32 đã khởi động và kết nối thành công!", "");
}

void loop() {
  // 1. Kiểm tra xem có ai nhắn lệnh điều khiển đèn không
  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("Có tin nhắn lệnh mới!");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  // 2. Kiểm tra cảm biến chuyển động (PIR)
  int currentPirState = digitalRead(motionSensor);
  if (currentPirState == HIGH) { // Nếu phát hiện có chuyển động
    if (pirState == LOW) { // Chỉ gửi tin nhắn 1 lần lúc mới phát hiện
      Serial.println("Phát hiện chuyển động!");
      bot.sendMessage(CHAT_ID, "🚨 CẢNH BÁO: Phát hiện có người đột nhập!", "");
      pirState = HIGH; // Cập nhật trạng thái
    }
  } else {
    if (pirState == HIGH) { // Khi không còn chuyển động nữa
      Serial.println("An toàn. Hết chuyển động.");
      pirState = LOW; // Reset trạng thái
    }
  }
}