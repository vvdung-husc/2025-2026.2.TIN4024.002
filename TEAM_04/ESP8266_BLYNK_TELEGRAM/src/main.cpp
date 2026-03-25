/*
 * THÔNG TIN NHÓM 04.002
 * 1. Nguyễn Quốc Bảo
 * 2. Trần Đức THăng

 */

#define BLYNK_TEMPLATE_ID "TMPL6zRd8Eq1S"
#define BLYNK_TEMPLATE_NAME "ESP32 LED TM1637"
#define BLYNK_AUTH_TOKEN "WetK2uutcfcXvUFOyxBwbPLP4gZHQdPi"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// WIFI
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// TELEGRAM
#define BOT_TOKEN "8270443543:AAGNMrYrG1cARaeTiWUK6EdSMeVy0ISEe6o"
#define CHAT_ID "5801101245"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// PIN
#define LED 2
#define DHTPIN 4
#define MQ2 34

DHT dht(DHTPIN, DHT22);

unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);

  // WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Dang ket noi WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");

  // Telegram trước (QUAN TRỌNG)
  client.setInsecure();

  // Test Telegram trước
  bot.sendMessage(CHAT_ID, "ESP32 da khoi dong!", "");

  Serial.println("Telegram OK");

  // Blynk KHÔNG block
  Blynk.config(BLYNK_AUTH_TOKEN);

  // Không bắt buộc connect ngay
  Blynk.connect(1000); // timeout 1s

  dht.begin();

  Serial.println("He thong san sang!");
}

// TELEGRAM HANDLE
void handleTelegram(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;

    // 👉 THÊM CÁI NÀY
    if (text == "/start") {
      String welcome = "🤖 ESP32 BOT\n\n";
      welcome += "/led_on - Bat den\n";
      welcome += "/led_off - Tat den\n";
      welcome += "/status - Xem du lieu";

      bot.sendMessage(CHAT_ID, welcome, "");
    }

    if (text == "/led_on") {
      digitalWrite(LED, HIGH);
      bot.sendMessage(CHAT_ID, "LED ON", "");
    }

    if (text == "/led_off") {
      digitalWrite(LED, LOW);
      bot.sendMessage(CHAT_ID, "LED OFF", "");
    }

    if (text == "/status") {
      float t = dht.readTemperature();
      float h = dht.readHumidity();
      int gas = analogRead(MQ2);

      String msg = "Nhiet do: " + String(t) + " C\n";
      msg += "Do am: " + String(h) + " %\n";
      msg += "Gas: " + String(gas);

      bot.sendMessage(CHAT_ID, msg, "");
    }
  }
}
void loop() {
  Blynk.run();

  if (millis() - lastTime > 1000) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      handleTelegram(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    lastTime = millis();
  }
}