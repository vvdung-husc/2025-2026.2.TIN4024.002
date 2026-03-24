#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Blynk
char auth[] = "BLYNK_AUTH_TOKEN";
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Telegram
#define BOTtoken "YOUR_TELEGRAM_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Blynk.begin(auth, ssid, pass);

  client.setInsecure();

  bot.sendMessage(CHAT_ID, "ESP8266 da ket noi thanh cong!", "");
}

void loop() {
  Blynk.run();
}
