/*
 * THÔNG TIN NHÓM 04.002
 * 1. Nguyễn Quốc Bảo
 * 2. Trần Đức THăng

 */

#define BLYNK_TEMPLATE_ID "TMPLxxxx"
#define BLYNK_TEMPLATE_NAME "ESP32 BLYNK"
#define BLYNK_AUTH_TOKEN "YOUR_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

// WIFI
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// TELEGRAM
#define BOT_TOKEN "8270443543:AAGNMrYrG1cARaeTiWUK6EdSMeVy0ISEe6o"
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// DHT
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// PIN
#define LED_PIN 2
#define MQ2_PIN 34

// BIẾN
float temp = 0;
float hum = 0;
int gas = 0;
bool ledState = false;

unsigned long lastTelegramCheck = 0;
unsigned long lastSend = 0;

BlynkTimer timer;

//////////////////////////////////////////////////
// BLYNK CONTROL LED
BLYNK_WRITE(V0) {
  ledState = param.asInt();
  digitalWrite(LED_PIN, ledState);
}
//////////////////////////////////////////////////

// ĐỌC SENSOR + GỬI BLYNK
void sendData() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  gas = analogRead(MQ2_PIN);

  if (isnan(temp) || isnan(hum)) return;

  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, hum);
  Blynk.virtualWrite(V3, gas);
  Blynk.virtualWrite(V4, millis()/1000);
}

//////////////////////////////////////////////////
// OLED
void displayOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);

  display.println("Team 01");
  display.print("Temp: "); display.println(temp);
  display.print("Hum: "); display.println(hum);
  display.print("Gas: "); display.println(gas);
  display.print("LED: ");
  display.println(ledState ? "ON" : "OFF");

  display.display();
}
//////////////////////////////////////////////////

// TELEGRAM
void handleTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      String chat_id = bot.messages[i].chat_id;
      String text = bot.messages[i].text;

      if (text == "/led_on") {
        digitalWrite(LED_PIN, HIGH);
        ledState = true;
        bot.sendMessage(chat_id, "LED ON", "");
      }

      if (text == "/led_off") {
        digitalWrite(LED_PIN, LOW);
        ledState = false;
        bot.sendMessage(chat_id, "LED OFF", "");
      }

      if (text == "/led_status") {
        bot.sendMessage(chat_id, ledState ? "LED ON" : "LED OFF", "");
      }

      if (text == "/get_weather") {
        String msg = "Nhiet do: " + String(temp) +
                     "\nDo am: " + String(hum) +
                     "\nGas: " + String(gas);
        bot.sendMessage(chat_id, msg, "");
      }
    }

    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}
//////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  dht.begin();

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // WIFI
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  client.setInsecure();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendData);
}

//////////////////////////////////////////////////

void loop() {
  Blynk.run();
  timer.run();

  displayOLED();

  // TELEGRAM check mỗi 2s
  if (millis() - lastTelegramCheck > 2000) {
    handleTelegram();
    lastTelegramCheck = millis();
  }
}