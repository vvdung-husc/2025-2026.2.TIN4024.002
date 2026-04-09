/*
THÔNG TIN NHÓM 01
1. Trần Thị Như Sang
2. Nguyễn Huy Phong
3. Đào Hữu Khánh
4. Lê Trọng Kha
5. Lê Quang Duy
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>

// ===== WIFI =====
char ssid[] = "YOUR_WIFI";   // wifi
char pass[] = "YOUR_PASS";   // pass

// ===== BLYNK =====
char auth[] = "YOUR_BLYNK_TOKEN"; // token

// ===== TELEGRAM =====
#define BOT_TOKEN "YOUR_BOT_TOKEN" // bot
#define CHAT_ID "YOUR_CHAT_ID"     // chat

WiFiClientSecure client;          // https
UniversalTelegramBot bot(BOT_TOKEN, client); // telegram

// === DHT ===
#define DHTPIN D4      // pin
#define DHTTYPE DHT11  // loại
DHT dht(DHTPIN, DHTTYPE);

// ==== LED ====
#define LED D2
bool ledState = false; // trạng thái

// ===== TIME =====
unsigned long startTime; // bắt đầu

// === BLYNK ===
BlynkTimer timer; // hẹn giờ

// ===== SEND TELEGRAM =====
void sendTelegram(String msg) {
  bot.sendMessage(CHAT_ID, msg, ""); // gửi
}

// ==== HANDLE TELEGRAM ====
void handleTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1); // đọc tin

  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text; // nội dung

    if (text == "/led_on") {
      digitalWrite(LED, HIGH);
      ledState = true;
      sendTelegram("LED ON");
    }
    else if (text == "/led_off") {
      digitalWrite(LED, LOW);
      ledState = false;
      sendTelegram("LED OFF");
    }
    else if (text == "/led_status") {
      sendTelegram(ledState ? "LED đang bật" : "LED đang tắt");
    }
    else if (text == "/get_weather") {
      float t = dht.readTemperature(); // nhiệt độ
      float h = dht.readHumidity();    // độ ẩm

      String msg = "🌡 Nhiệt độ: " + String(t) +
                   "\n💧 Độ ẩm: " + String(h);
      sendTelegram(msg);
    }
  }
}

// ===== BLYNK LED CONTROL =====
BLYNK_WRITE(V0) {
  int value = param.asInt(); // đọc app
  digitalWrite(LED, value);
  ledState = value;
}

// ==== SEND DATA ====
void sendData() {
  float t = dht.readTemperature(); // đọc nhiệt
  float h = dht.readHumidity();    // đọc ẩm

  // Nếu không có DHT → random
  if (isnan(t) || isnan(h)) {
    t = random(25, 35);
    h = random(60, 90);
  }

  Blynk.virtualWrite(V1, t); // gửi Blynk
  Blynk.virtualWrite(V2, h);

  static float lastT = 0;
  static float lastH = 0;

  if (abs(t - lastT) > 1 || abs(h - lastH) > 3) {
    sendTelegram("📢 Thay đổi môi trường\n🌡 " + String(t) + "\n💧 " + String(h));
    lastT = t;
    lastH = h;
  }
}

// ===== UPTIME =====
void sendUptime() {
  unsigned long uptime = (millis() - startTime) / 1000; // giây
  Blynk.virtualWrite(V3, uptime);
}

// ===== SETUP =====
void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  WiFi.begin(ssid, pass);
  client.setInsecure(); // https

  Blynk.begin(auth, ssid, pass);
  dht.begin();

  startTime = millis();

  timer.setInterval(5000L, sendData);   // 5s
  timer.setInterval(1000L, sendUptime); // 1s
}

// ===== LOOP =====
void loop() {
  Blynk.run();
  timer.run();
  handleTelegram(); // xử lý bot
}
