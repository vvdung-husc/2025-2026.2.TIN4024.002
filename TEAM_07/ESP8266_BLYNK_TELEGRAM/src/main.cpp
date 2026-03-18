/*
 * THÔNG TIN NHÓM XY.002
 * 1. Nguyễn Văn Bình
 * 2. Lê Nguyễn Hương Nguyên
 * 3. ...
 */

#include <Arduino.h>

// Tự động chọn thư viện WiFi theo Board
#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiClientSecure.h>
  #include <BlynkSimpleEsp32.h>
#else
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h>
  #include <BlynkSimpleEsp8266.h>
#endif

#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>

// --- CẤU HÌNH ---
char auth[] = "TOKEN_BLYNK_CUA_BAN";
char ssid[] = "Wokwi-GUEST"; 
char pass[] = "";
#define BOTtoken "TOKEN_TELEGRAM_CUA_BAN"

#define DHTPIN 15
#define DHTTYPE DHT22
#define LED_PIN 2

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(128, 64, &Wire, -1);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
BlynkTimer timer;

// Hàm gửi dữ liệu lên Blynk và OLED
void sendSensorData() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int gas = random(100, 500); // Sinh ngẫu nhiên theo yêu cầu

  Blynk.virtualWrite(V1, t); // Nhiệt độ
  Blynk.virtualWrite(V2, h); // Độ ẩm
  Blynk.virtualWrite(V3, gas); // Khí Gas
  Blynk.virtualWrite(V4, millis() / 1000); // Uptime

  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Temp: %.1f C\nHum: %.1f %%\nGas: %d\nUptime: %lds", t, h, gas, millis()/1000);
  display.display();
}

// Điều khiển LED từ Blynk
BLYNK_WRITE(V0) {
  digitalWrite(LED_PIN, param.asInt());
}

// Xử lý Telegram
void handleTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;
    String chat_id = bot.messages[i].chat_id;

    if (text == "/led_on") { digitalWrite(LED_PIN, HIGH); bot.sendMessage(chat_id, "LED ON"); }
    if (text == "/led_off") { digitalWrite(LED_PIN, LOW); bot.sendMessage(chat_id, "LED OFF"); }
    if (text == "/led_status") { bot.sendMessage(chat_id, digitalRead(LED_PIN) ? "LED is ON" : "LED is OFF"); }
    if (text == "/get_weather") { 
      bot.sendMessage(chat_id, "Temp: " + String(dht.readTemperature()) + "C"); 
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  Blynk.begin(auth, ssid, pass);
  client.setInsecure(); // Cần thiết cho Telegram
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
  static unsigned long lastBot;
  if (millis() - lastBot > 1000) { handleTelegram(); lastBot = millis(); }
}