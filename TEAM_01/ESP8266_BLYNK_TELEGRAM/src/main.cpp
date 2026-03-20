/*
 * ========================================
 * DỰ ÁN: ESP8266_BLYNK_TELEGRAM
 * ========================================
 * THÔNG TIN NHÓM X
 * 1. Nguyễn Duy Phong
 * 2. [Tên thành viên 2]
 * 3. [Tên thành viên 3]
 * ========================================
 * Mô tả: Hệ thống IoT tích hợp Blynk + Telegram
 * Board: ESP32 (Mô phỏng ESP8266)
 * ========================================
 */
// ===== CẤU HÌNH WIFI =====
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// ===== CẤU HÌNH BLYNK =====
#define BLYNK_TEMPLATE_ID "TMPL6AAfiU82_"           // Thay bằng Template ID của bạn
#define BLYNK_TEMPLATE_NAME "ESP8266 BLYNK TELEGRAM"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN_HERE"    // Thay bằng Auth Token của bạn

// Virtual Pins Blynk
#define VPIN_UPTIME       V0
#define VPIN_LED_SWITCH   V1
#define VPIN_TEMPERATURE  V2
#define VPIN_HUMIDITY     V3
#define VPIN_GAS          V4
#define VPIN_TEAM_INFO    V5

// ===== CẤU HÌNH TELEGRAM =====
#define BOT_TOKEN "YOUR_BOT_TOKEN_HERE"             // Thay bằng Bot Token
#define CHAT_ID   "YOUR_CHAT_ID_HERE"               // Thay bằng Group Chat ID

// ===== PINS =====
#define LED_PIN      2     // LED built-in
#define DHT_PIN      4     // DHT22 sensor
#define MQ2_PIN      34    // MQ2 analog (hoặc random)
#define OLED_SDA     21    // I2C SDA
#define OLED_SCL     22    // I2C SCL

// ===== CẤU HÌNH DHT =====
#define DHTTYPE DHT22

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>


DHT dht(DHT_PIN, DHTTYPE);

// ===== CẤU HÌNH OLED =====
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// ===== TELEGRAM =====
WiFiClientSecure clientTelegram;
UniversalTelegramBot bot(BOT_TOKEN, clientTelegram);

// ===== BLYNK =====
BlynkTimer timer;

// ===== BIẾN TOÀN CỤC =====
float temperature = 0.0;
float humidity = 0.0;
int gasValue = 0;
bool ledState = false;
unsigned long uptime = 0;

bool USE_MOCK_GAS = true;  // true = random gas, false = đọc từ MQ2

unsigned long lastTelegramCheck = 0;
const long telegramInterval = 1000;  // Check Telegram mỗi 1 giây

float lastTemp = -999;
float lastHum = -999;

// ===== KHAI BÁO HÀM =====
void updateOLED();
void sendToBlynk();
void handleTelegramMessages(int numNewMessages);
void sendTelegramNotification(String message);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========================================");
  Serial.println("  ESP8266_BLYNK_TELEGRAM");
  Serial.println("  NHÓM X");
  Serial.println("  1. Nguyễn Duy Phong");
  Serial.println("  2. [Thành viên 2]");
  Serial.println("  3. [Thành viên 3]");
  Serial.println("========================================");

  // ===== LED =====
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  ledState = false;
  Serial.println("✓ LED initialized");

  // ===== DHT22 =====
  dht.begin();
  Serial.println("✓ DHT22 initialized");

  // ===== MQ2 =====
  if (!USE_MOCK_GAS) {
    pinMode(MQ2_PIN, INPUT);
  }
  Serial.println(USE_MOCK_GAS ? "✓ MQ2 (Random Mode)" : "✓ MQ2 (Sensor Mode)");

  // ===== OLED =====
  Wire.begin(OLED_SDA, OLED_SCL);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "ESP32 IoT");
  u8g2.drawStr(0, 25, "Team X");
  u8g2.drawStr(0, 40, "Starting...");
  u8g2.sendBuffer();
  Serial.println("✓ OLED initialized");

  // ===== WIFI =====
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✓ WiFi Connected!");
  Serial.print("  IP: ");
  Serial.println(WiFi.localIP());

  // ===== BLYNK =====
  Serial.print("Connecting to Blynk");
  Blynk.config(BLYNK_AUTH_TOKEN);
  bool blynkOK = Blynk.connect(3000);
  if (blynkOK) {
    Serial.println("✓ Blynk Connected!");
  } else {
    Serial.println("✗ Blynk Failed (OK on Wokwi)");
  }

  // ===== TELEGRAM =====
  clientTelegram.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  Serial.println("✓ Telegram Bot initialized");

  // ===== BLYNK TIMER =====
  timer.setInterval(2000L, sendToBlynk);  // Gửi Blynk mỗi 2 giây

  Serial.println("\n========================================");
  Serial.println("System Ready!");
  Serial.println("========================================\n");

  // Gửi tin nhắn khởi động lên Telegram
  sendTelegramNotification("🚀 ESP32 Started!\nTeam X - IoT Project");
}

void loop() {
  if (Blynk.connected()) {
    Blynk.run();
  }
  timer.run();

  uptime = millis() / 1000;

  // ===== ĐỌC DHT22 =====
  float newTemp = dht.readTemperature();
  float newHum = dht.readHumidity();

  if (!isnan(newTemp) && !isnan(newHum)) {
    temperature = newTemp;
    humidity = newHum;

    // Gửi thông báo Telegram khi thay đổi > 0.5
    if (abs(temperature - lastTemp) > 0.5 || abs(humidity - lastHum) > 5) {
      String msg = "🌡️ Temperature: " + String(temperature, 1) + "°C\n";
      msg += "💧 Humidity: " + String(humidity, 1) + "%";
      sendTelegramNotification(msg);
      
      lastTemp = temperature;
      lastHum = humidity;
    }
  }

  // ===== ĐỌC MQ2 =====
  if (USE_MOCK_GAS) {
    gasValue = random(200, 800);  // Random 200-800
  } else {
    gasValue = analogRead(MQ2_PIN);
  }

  // ===== TELEGRAM BOT =====
  unsigned long currentMillis = millis();
  if (currentMillis - lastTelegramCheck > telegramInterval) {
    lastTelegramCheck = currentMillis;
    
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages) {
      handleTelegramMessages(numNewMessages);
    }
  }

  // ===== CẬP NHẬT OLED =====
  static unsigned long lastOLEDUpdate = 0;
  if (currentMillis - lastOLEDUpdate > 500) {
    lastOLEDUpdate = currentMillis;
    updateOLED();
  }

  delay(50);
}

// ===== CẬP NHẬT OLED =====
void updateOLED() {
  u8g2.clearBuffer();

  // Header
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "Team X - IoT");
  u8g2.drawLine(0, 12, 128, 12);

  // Temp & Hum
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 24, "T:");
  u8g2.setCursor(16, 24);
  u8g2.print(temperature, 1);
  u8g2.drawStr(50, 24, "C");

  u8g2.drawStr(65, 24, "H:");
  u8g2.setCursor(81, 24);
  u8g2.print(humidity, 1);
  u8g2.drawStr(115, 24, "%");

  // Gas
  u8g2.drawStr(0, 36, "Gas:");
  u8g2.setCursor(30, 36);
  u8g2.print(gasValue);

  // LED
  u8g2.drawStr(0, 48, "LED:");
  u8g2.drawStr(30, 48, ledState ? "ON" : "OFF");

  // Uptime
  u8g2.drawStr(0, 60, "Up:");
  u8g2.setCursor(24, 60);
  u8g2.print(uptime);
  u8g2.drawStr(60, 60, "s");

  u8g2.sendBuffer();
}

// ===== GỬI DỮ LIỆU LÊN BLYNK =====
void sendToBlynk() {
  if (Blynk.connected()) {
    Blynk.virtualWrite(VPIN_UPTIME, uptime);
    Blynk.virtualWrite(VPIN_LED_SWITCH, ledState);
    Blynk.virtualWrite(VPIN_TEMPERATURE, temperature);
    Blynk.virtualWrite(VPIN_HUMIDITY, humidity);
    Blynk.virtualWrite(VPIN_GAS, gasValue);
    Blynk.virtualWrite(VPIN_TEAM_INFO, "Team X - Phong");
  }
}

// ===== XỬ LÝ TELEGRAM MESSAGES =====
void handleTelegramMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    Serial.println("---");
    Serial.println("From: " + from_name);
    Serial.println("Message: " + text);

    if (text == "/start") {
      String welcome = "👋 Welcome to Team X IoT!\n\n";
      welcome += "Commands:\n";
      welcome += "/led_on - Turn LED ON\n";
      welcome += "/led_off - Turn LED OFF\n";
      welcome += "/led_status - Get LED status\n";
      welcome += "/get_weather - Get Temperature & Humidity\n";
      bot.sendMessage(chat_id, welcome, "");
    }
    else if (text == "/led_on") {
      ledState = true;
      digitalWrite(LED_PIN, HIGH);
      Blynk.virtualWrite(VPIN_LED_SWITCH, 1);
      bot.sendMessage(chat_id, "✅ LED is ON", "");
    }
    else if (text == "/led_off") {
      ledState = false;
      digitalWrite(LED_PIN, LOW);
      Blynk.virtualWrite(VPIN_LED_SWITCH, 0);
      bot.sendMessage(chat_id, "❌ LED is OFF", "");
    }
    else if (text == "/led_status") {
      String status = "💡 LED Status: ";
      status += ledState ? "ON ✅" : "OFF ❌";
      bot.sendMessage(chat_id, status, "");
    }
    else if (text == "/get_weather") {
      String weather = "🌡️ Temperature: " + String(temperature, 1) + "°C\n";
      weather += "💧 Humidity: " + String(humidity, 1) + "%\n";
      weather += "💨 Gas: " + String(gasValue);
      bot.sendMessage(chat_id, weather, "");
    }
    else {
      bot.sendMessage(chat_id, "❌ Unknown command. Send /start for help.", "");
    }
  }
}

// ===== GỬI THÔNG BÁO TELEGRAM =====
void sendTelegramNotification(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    bot.sendMessage(CHAT_ID, message, "");
  }
}

// ===== BLYNK HANDLERS =====
BLYNK_WRITE(V1) {
  ledState = param.asInt();
  digitalWrite(LED_PIN, ledState);
  Serial.print("Blynk LED: ");
  Serial.println(ledState ? "ON" : "OFF");
}

BLYNK_CONNECTED() {
  Serial.println("Blynk Connected!");
  Blynk.syncVirtual(V1);
}