/*
  ESP32 - Điều khiển LED qua Telegram + Cảnh báo chuyển động PIR
  Tác giả: Lê Trọng Kha - 22t1020634
  
  Chức năng:
  - /start       : Hiển thị hướng dẫn
  - /led_on      : Bật đèn LED
  - /led_off     : Tắt đèn LED
  - /get_state   : Lấy trạng thái đèn hiện tại
  - Tự động gửi cảnh báo khi PIR phát hiện chuyển động
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Cấu hình WiFi
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// Cấu hình Telegram
#define BOTtoken  "8606104161:AAFr76oGY0Z6JuuE6a7sI9i63aIgHUOdH5s"
#define CHAT_ID   "7200300525"

// Chân GPIO
const int PIN_PIR = 27;
const int PIN_LED = 23;

// Biến toàn cục
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

volatile bool motionDetected = false;
bool ledState = false;

const unsigned long BOT_MTBS = 1000;
unsigned long lastTimeBotRan = 0;

// Interrupt cho PIR
void IRAM_ATTR detectsMovement() {
  motionDetected = true;
}

// Điều khiển LED
void setLED(bool on) {
  ledState = on;
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

// Xử lý lệnh Telegram
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text    = bot.messages[i].text;
    String from    = bot.messages[i].from_name;

    Serial.print("Lệnh từ "); Serial.print(from);
    Serial.print(": "); Serial.println(text);

    if (text == "/start") {
      String welcome = "Xin chào, " + from + ".\n";
      welcome += "Sử dụng các lệnh sau để điều khiển đèn LED.\n\n";
      welcome += "Gửi /led_on bật sáng đèn\n";
      welcome += "Gửi /led_off để tắt đèn\n";
      welcome += "Gửi /get_state để yêu cầu trạng thái đèn hiện tại";
      bot.sendMessage(chat_id, welcome);
    }
    else if (text == "/led_on") {
      setLED(true);
      bot.sendMessage(chat_id, "LED bật sáng");
    }
    else if (text == "/led_off") {
      setLED(false);
      bot.sendMessage(chat_id, "LED tắt");
    }
    else if (text == "/get_state") {
      bot.sendMessage(chat_id, ledState ? "LED is ON" : "LED is OFF");
    }
    else {
      bot.sendMessage(chat_id, "Lệnh không hợp lệ. Gửi /start để xem hướng dẫn.");
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_PIR, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  attachInterrupt(digitalPinToInterrupt(PIN_PIR), detectsMovement, RISING);

  Serial.print("Đang kết nối WiFi: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi đã kết nối. IP: " + WiFi.localIP().toString());

  // Bỏ qua tin nhắn cũ
  int numMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numMessages > 0) {
    numMessages = bot.getUpdates(bot.last_message_received + 1);
  }
  Serial.println("Đã bỏ qua tin nhắn cũ. Sẵn sàng nhận lệnh mới.");

  // Gửi thông báo khởi động
  String startMsg = "IoT Developer started up\n\n";
  startMsg += "Sử dụng các lệnh sau để điều khiển đèn LED:\n";
  startMsg += "Gửi /led_on bật sáng đèn\n";
  startMsg += "Gửi /led_off để tắt đèn\n";
  startMsg += "Gửi /get_state để yêu cầu trạng thái đèn hiện tại";
  bot.sendMessage(CHAT_ID, startMsg);
}

void loop() {
  // Xử lý PIR
  if (motionDetected) {
    motionDetected = false;
    Serial.println("MOTION DETECTED!");
    bot.sendMessage(CHAT_ID, "⚠️ Phát hiện chuyển động!");
  }

  // Đọc tin nhắn mới từ Telegram
  if (millis() - lastTimeBotRan > BOT_MTBS) {
    lastTimeBotRan = millis();
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages > 0) {
      handleNewMessages(numNewMessages);
    }
  }
}
