#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// ===== CẤU HÌNH WiFi =====
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ===== CẤU HÌNH TELEGRAM =====
#define BOTtoken  "8648129752:AAGx0vwhD2u4mO7Xs1iui2qkqlorC5KWnbs"
#define CHAT_ID   "8475654930"

// ===== CHÂN PHẦN CỨNG =====
#define PIR_PIN  27
#define LED_PIN  23

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// ===== BIẾN =====
unsigned long lastTimeBotRan = 0;
unsigned long lastPIRAlert   = 0;
const int botRequestDelay    = 1000;
const int pirAlertDelay      = 5000;
bool ledState                = false;

// ===== XỬ LÝ TIN NHẮN =====
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text    = bot.messages[i].text;
    String from    = bot.messages[i].from_name;

    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Bạn không có quyền dùng bot này.", "");
      continue;
    }

    Serial.println("Lệnh nhận được: " + text);

    if (text == "/start") {
      String welcome = "Xin chào, " + from + "!\n\n";
      welcome += "🤖 Hệ thống giám sát chuyển động\n\n";
      welcome += "Các lệnh có sẵn:\n";
      welcome += "/start - Khởi động\n";
      welcome += "/status - Kiểm tra trạng thái\n";
      welcome += "/ledon - Bật LED\n";
      welcome += "/ledoff - Tắt LED\n";
      welcome += "/pir - Kiểm tra cảm biến PIR\n";
      bot.sendMessage(chat_id, welcome, "");
    }
    else if (text == "/status") {
      String status = "📊 Trạng thái hệ thống:\n";
      status += "💡 LED: " + String(ledState ? "BẬT ✅" : "TẮT ❌") + "\n";
      status += "🚶 PIR: " + String(digitalRead(PIR_PIN) ? "Có chuyển động!" : "Không có chuyển động") + "\n";
      bot.sendMessage(chat_id, status, "");
    }
    else if (text == "/ledon") {
      digitalWrite(LED_PIN, HIGH);  // ✅ GPIO 23
      ledState = true;
      bot.sendMessage(chat_id, "💡 LED đã BẬT", "");
    }
    else if (text == "/ledoff") {
      digitalWrite(LED_PIN, LOW);   // ✅ GPIO 23
      ledState = false;
      bot.sendMessage(chat_id, "🌑 LED đã TẮT", "");
    }
    else if (text == "/pir") {
      bool motion = digitalRead(PIR_PIN);
      bot.sendMessage(chat_id, motion ? "🚶 Phát hiện chuyển động!" : "✅ Không có chuyển động", "");
    }
    else {
      bot.sendMessage(chat_id, "❓ Lệnh không hợp lệ. Gõ /start để xem danh sách.", "");
    }
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(PIR_PIN, INPUT);

  // Kết nối WiFi
  WiFi.disconnect(true);
  delay(500);
  WiFi.mode(WIFI_STA);
  delay(500);

  Serial.print("Đang kết nối WiFi");
  WiFi.begin(ssid, password);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    timeout++;
    if (timeout > 30) {
      Serial.println("\n❌ Không kết nối được WiFi! Khởi động lại...");
      ESP.restart();
    }
  }

  Serial.println("\n✅ WiFi đã kết nối: " + WiFi.localIP().toString());

  client.setInsecure();

  // Xóa tin nhắn tồn đọng
  Serial.println("Đang xóa tin nhắn cũ...");
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages) {
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  Serial.println("🤖 Bot Telegram sẵn sàng!");
}

// ===== LOOP =====
void loop() {
  // Kiểm tra PIR - tự động gửi cảnh báo
  if (digitalRead(PIR_PIN) == HIGH) {
    if (millis() - lastPIRAlert > pirAlertDelay) {
      bot.sendMessage(CHAT_ID, "🚨 CẢNH BÁO: Phát hiện chuyển động!", "");
      digitalWrite(LED_PIN, HIGH);  // Tự bật LED khi có chuyển động
      ledState = true;
      lastPIRAlert = millis();
      Serial.println("PIR: Phát hiện chuyển động!");
    }
  }

  // Nhận lệnh từ Telegram
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}