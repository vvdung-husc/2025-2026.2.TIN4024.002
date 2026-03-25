#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";


#define BOT_TOKEN "8265224208:AAFm0lSBmdw5paZ2HftkYARLoaTjm-mc79o" 
#define CHAT_ID "7054661575"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

const int ledPin = 22; 
const int pirPin = 27; 

bool ledState = false;
bool motionDetected = false;
int botRequestDelay = 1000; 
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Bạn không có quyền điều khiển bot này!", "");
      continue;
    }

    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Xin chào, " + from_name + ".\n";
      welcome += "Sử dụng các lệnh sau để điều khiển đèn LED.\n\n";
      welcome += "Gửi /led_on bật sáng đèn\n";
      welcome += "Gửi /led_off để tắt đèn\n";
      welcome += "Gửi /get_state để yêu cầu trạng thái đèn hiện tại";
      bot.sendMessage(chat_id, welcome, "");
    } 
    else if (text == "/led_on") {
      ledState = true;
      digitalWrite(ledPin, HIGH);
      bot.sendMessage(chat_id, "LED bật sáng", "");
    } 
    else if (text == "/led_off") {
      ledState = false;
      digitalWrite(ledPin, LOW);
      bot.sendMessage(chat_id, "LED is OFF", "");
    } 
    else if (text == "/get_state") {
      if (ledState) {
        bot.sendMessage(chat_id, "LED is ON", "");
      } else {
        bot.sendMessage(chat_id, "LED is OFF", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  digitalWrite(ledPin, LOW);


  Serial.print("Đang kết nối WiFi");
  WiFi.begin(ssid, password);
  client.setInsecure(); 

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi đã kết nối!");
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  int pirState = digitalRead(pirPin);
  if (pirState == HIGH && !motionDetected) {
    Serial.println("Phát hiện chuyển động!");
    bot.sendMessage(CHAT_ID, "Cảnh báo: Phát hiện có chuyển động!", "");
    motionDetected = true;
  } else if (pirState == LOW && motionDetected) {
    motionDetected = false;
  }
}