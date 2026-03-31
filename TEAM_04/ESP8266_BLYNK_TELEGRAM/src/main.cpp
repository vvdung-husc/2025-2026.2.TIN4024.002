/*
 * THÔNG TIN NHÓM 04.002
 * 1. Nguyễn Quốc Bảo
 * 2. Trần Đức Thăng
 */

#define BLYNK_TEMPLATE_ID "TMPL6vTNEPSnD"
#define BLYNK_TEMPLATE_NAME "ESP32 IoT"
#define BLYNK_AUTH_TOKEN "pT9W2HiuIpmNl6Ifs0XtY8HM4mc9hDdx"

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
#define CHAT_ID "-5277488606"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// PIN
#define LED 2
#define DHTPIN 4
#define MQ2 34

DHT dht(DHTPIN, DHT22);

// TIMER
unsigned long lastTelegram = 0;
unsigned long lastBlynk = 0;

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setInsecure();

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  dht.begin();

  bot.sendMessage(CHAT_ID, "ESP32 da khoi dong!", "");
}

// ===== BLYNK CONTROL =====
BLYNK_WRITE(V0) {
  int value = param.asInt();
  digitalWrite(LED, value);
}

// ===== TELEGRAM =====
void handleTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  for (int i = 0; i < numNewMessages; i++) {

    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    // fix @bot
    if (text.indexOf("@") != -1) {
      text = text.substring(0, text.indexOf("@"));
    }

    // START
    if (text == "/start") {
      bot.sendMessage(chat_id,
        "🤖 ESP32 BOT\n\n"
        "/led_on\n"
        "/led_off\n"
        "/led_status\n"
        "/get_weather", "");
    }

    // LED ON
    else if (text == "/led_on") {
      digitalWrite(LED, HIGH);
      Blynk.virtualWrite(V0, 1);
      bot.sendMessage(chat_id, "LED ON", "");
    }

    // LED OFF
    else if (text == "/led_off") {
      digitalWrite(LED, LOW);
      Blynk.virtualWrite(V0, 0);
      bot.sendMessage(chat_id, "LED OFF", "");
    }

    // STATUS
    else if (text == "/led_status") {
      String st = digitalRead(LED) ? "ON" : "OFF";
      bot.sendMessage(chat_id, "Trang thai LED: " + st, "");
    }

    // WEATHER
    else if (text == "/get_weather") {

      float t = dht.readTemperature();
      float h = dht.readHumidity();

      // 🔥 FIX GAS (KHÔNG DÙNG analogRead nữa)
      int gas = random(200, 800);

      if (isnan(t)) t = random(25, 35);
      if (isnan(h)) h = random(60, 90);

      String msg = "🌤 Thong tin hien tai:\n";
      msg += "🌡 Nhiet do: " + String(t) + " C\n";
      msg += "💧 Do am: " + String(h) + " %\n";
      msg += "🔥 Gas: " + String(gas);

      bot.sendMessage(chat_id, msg, "");

      // 👉 gửi lên Blynk
      Blynk.virtualWrite(V1, t);
      Blynk.virtualWrite(V2, h);
      Blynk.virtualWrite(V4, gas);
    }

    bot.last_message_received = bot.messages[i].update_id;
  }
}

// ===== LOOP =====
void loop() {
  Blynk.run();

  // TELEGRAM mỗi 1s
  if (millis() - lastTelegram > 1000) {
    handleTelegram();
    lastTelegram = millis();
  }

  // ===== BLYNK AUTO UPDATE =====
  if (millis() - lastBlynk > 2000) {

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // 🔥 GAS RANDOM
    int gas = random(200, 800);

    if (isnan(t)) t = random(25, 35);
    if (isnan(h)) h = random(60, 90);

    Blynk.virtualWrite(V1, t);
    Blynk.virtualWrite(V2, h);
    Blynk.virtualWrite(V4, gas);

    // ⏱ TIME chạy mượt
    Blynk.virtualWrite(V3, millis() / 1000);

    lastBlynk = millis();
  }
}