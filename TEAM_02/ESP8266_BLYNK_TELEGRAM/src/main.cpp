/*
  THÔNG TIN NHÓM 02
  1. Mai Anh Quân
  2. Nguyễn Trọng Vĩ
  3. Lê Văn Quốc Dũng
  4. Nguyễn Văn Khải
*/


#define BLYNK_TEMPLATE_ID "TMPL123456"
#define BLYNK_TEMPLATE_NAME "ESP8266_BLYNK_TELEGRAM"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define LED_PIN LED_BUILTIN

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

char auth[] = "BLYNK_AUTH_TOKEN";

#define BOTtoken "TELEGRAM_BOT_TOKEN"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Blynk.begin(auth, ssid, pass);
  client.setInsecure();
}

void loop() {
  Blynk.run();

  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      String text = bot.messages[i].text;
      String chat_id = bot.messages[i].chat_id;

      if (text == "/on") {
        digitalWrite(LED_PIN, LOW);
        bot.sendMessage(chat_id, "LED ON", "");
      }

      if (text == "/off") {
        digitalWrite(LED_PIN, HIGH);
        bot.sendMessage(chat_id, "LED OFF", "");
      }
    }
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}
