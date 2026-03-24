/*
 * ========================================
 * DỰ ÁN: ESP8266_BLYNK_TELEGRAM
 * NHÓM 08.002
 * 1. Lê Văn Hoàng Thái - Telegram: @kierannn05
 * 2. Hoàng Phi Long - Telegram: @chocopie
 * 3. Lê Phước Phúc - Telegram: @davidsea04
 * ========================================
 *
 * MÔ TẢ DỰ ÁN:
 * Hệ thống IoT thông minh kết hợp Blynk và Telegram Bot
 * để giám sát và điều khiển thiết bị từ xa
 *
 * CHỨC NĂNG CHÍNH:
 * - Giám sát nhiệt độ, độ ẩm qua cảm biến DHT22
 * - Phát hiện chuyển động qua cảm biến PIR
 * - Đo nồng độ khí gas
 * - Điều khiển LED và 2 Relay
 * - Hiển thị thông tin trên màn hình OLED SSD1306
 * - Điều khiển từ xa qua Blynk App
 * - Nhận lệnh và gửi cảnh báo qua Telegram Bot
 *
 * LỆNH TELEGRAM:
 * /start       - Hiển thị menu hướng dẫn
 * /led_on      - Bật đèn LED
 * /led_off     - Tắt đèn LED
 * /led_status  - Kiểm tra trạng thái LED
 * /get_weather - Lấy thông tin nhiệt độ, độ ẩm, khí gas
 *
 * CÔNG NGHỆ SỬ DỤNG:
 * - ESP32 DevKit V4
 * - Blynk IoT Platform
 * - Telegram Bot API
 * - DHT22 (Temperature & Humidity)
 * - PIR Motion Sensor
 * - SSD1306 OLED Display (I2C)
 * - WiFi Communication
 *
 * ⚠️  TRƯỚC MỖI LẦN CHẠY — mở trình duyệt vào link này để xoá tin cũ:
 * https://api.telegram.org/bot8674058851:AAGxER1vncD5YHzr0LZIl65AYq0Mnj0Q3XI/deleteWebhook?drop_pending_updates=true
 * Thấy {"ok":true} là được, rồi mới chạy Wokwi.
 *
 * ========================================
 */

#define BLYNK_TEMPLATE_ID "TMPL6ls3ZWYnQ"
#define BLYNK_TEMPLATE_NAME "tele"
#define BLYNK_AUTH_TOKEN "1I8fU5UQ-nbkFdZodi7xRql5OzLBOybj"

#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h>

// ===== WIFI =====
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ===== TELEGRAM =====
#define BOTtoken "8708472451:AAEdh58-pAV8GJYDaEqLW5G5kJ_N9NSefrQ"
#define CHAT_ID  "-4996649495"   

// ===== PINS =====
#define PIN_LED     2
#define PIN_DHT     4
#define PIN_PIR    27
#define PIN_RELAY1 25
#define PIN_RELAY2 26
#define PIN_SDA    21
#define PIN_SCL    22

// ===== OBJECTS =====
#define DHTTYPE DHT22
DHT dht(PIN_DHT, DHTTYPE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
BlynkTimer blynkTimer;

// ===== VARIABLES =====
float temperature = 0.0;
float humidity    = 0.0;
int   gasValue    = 0;
bool  ledState    = false;
bool  pirState    = false;
unsigned long uptime = 0;

const unsigned long BOT_MTBS = 1000;
unsigned long lastTimeBotRan = 0;

// ===================================================
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text    = bot.messages[i].text;
    String from    = bot.messages[i].from_name;

    Serial.println("[TG] " + from + ": " + text);

    if (text == "/start") {
      String msg = "Xin chao " + from + "!\n\n";
      msg += "/led_on      - Bat den LED\n";
      msg += "/led_off     - Tat den LED\n";
      msg += "/led_status  - Trang thai LED\n";
      msg += "/get_weather - Nhiet do & do am\n\n";
      msg += "Team 01.002: Phong, Khanh, Sang, Kha, Duy";
      bot.sendMessage(chat_id, msg, "");
    }
    else if (text == "/led_on") {
      ledState = true;
      digitalWrite(PIN_LED, HIGH);
      Blynk.virtualWrite(V1, 1);
      bot.sendMessage(chat_id, "LED Bat", "");
      Serial.println("[LED] ON");
    }
    else if (text == "/led_off") {
      ledState = false;
      digitalWrite(PIN_LED, LOW);
      Blynk.virtualWrite(V1, 0);
      bot.sendMessage(chat_id, "LED Tat", "");
      Serial.println("[LED] OFF");
    }
    else if (text == "/led_status") {
      bot.sendMessage(chat_id, ledState ? "LED is ON" : "LED is OFF", "");
    }
    else if (text == "/get_weather") {
      String w  = "Nhiet do is " + String(temperature, 1) + " C\n";
      w += "Do am    is " + String(humidity, 1) + " %\n";
      w += "Khi gas  is " + String(gasValue);
      bot.sendMessage(chat_id, w, "");
    }
    else {
      if (text.startsWith("/")) {
        bot.sendMessage(chat_id, "Lenh khong hop le. Gui /start de xem danh sach.", "");
      }
    }
  }
}

// ===================================================
void sendToBlynk() {
  if (!Blynk.connected()) return;
  Blynk.virtualWrite(V0, uptime);
  Blynk.virtualWrite(V1, ledState ? 1 : 0);
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3, humidity);
  Blynk.virtualWrite(V4, gasValue);
  Blynk.virtualWrite(V5, "Thái, Long, Đạt, Phúc, Linh");
}

// ===================================================
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n========================================");
  Serial.println("  ESP32 BLYNK+TELEGRAM - NHOM 08");
  Serial.println("========================================");

  pinMode(PIN_LED,    OUTPUT); digitalWrite(PIN_LED,    LOW);
  pinMode(PIN_PIR,    INPUT);
  pinMode(PIN_RELAY1, OUTPUT); digitalWrite(PIN_RELAY1, LOW);
  pinMode(PIN_RELAY2, OUTPUT); digitalWrite(PIN_RELAY2, LOW);

  dht.begin();
  Wire.begin(PIN_SDA, PIN_SCL);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "Team 08");
  u8g2.drawStr(0, 25, "Connecting...");
  u8g2.sendBuffer();

  Serial.print("Ket noi WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\n[OK] WiFi: " + WiFi.localIP().toString());

  Blynk.config(BLYNK_AUTH_TOKEN);
  if (Blynk.connect(3000)) Serial.println("[OK] Blynk");
  else                      Serial.println("[??] Blynk - se thu lai");

  // Bỏ qua tin nhắn cũ
  Serial.println("[TG] Bo qua tin nhan cu...");
  int n = bot.getUpdates(bot.last_message_received + 1);
  while (n > 0) n = bot.getUpdates(bot.last_message_received + 1);
  Serial.println("[OK] Telegram san sang");

  blynkTimer.setInterval(2000L, sendToBlynk);

  Serial.println("========================================");
  Serial.println("  System Ready!");
  Serial.println("========================================\n");

  String startMsg = "ESP32 Team 08 khoi dong!\n\n";
  startMsg += "/led_on      - Bat den LED\n";
  startMsg += "/led_off     - Tat den LED\n";
  startMsg += "/led_status  - Trang thai LED\n";
  startMsg += "/get_weather - Nhiet do & do am\n\n";
  startMsg += "Team 08.002: Thái, Long, Đạt, Phúc, Linh";
  bool sent = bot.sendMessage(CHAT_ID, startMsg, "");
  Serial.println(sent ? "[OK] Gui tin khoi dong" : "[FAIL] Gui tin - kiem tra CHAT_ID");
}

// ===================================================
void loop() {
  if (Blynk.connected()) Blynk.run();
  blynkTimer.run();

  uptime = millis() / 1000;

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t) && !isnan(h)) { temperature = t; humidity = h; }

  static unsigned long lastGas = 0;
  if (millis() - lastGas > 3000) { lastGas = millis(); gasValue = random(200, 800); }

  bool pir = digitalRead(PIN_PIR);
  if (pir != pirState) {
    pirState = pir;
    if (pir) {
      Serial.println("[PIR] Motion!");
      bot.sendMessage(CHAT_ID, "Phat hien chuyen dong!", "");
    }
  }

  if (millis() - lastTimeBotRan > BOT_MTBS) {
    lastTimeBotRan = millis();
    int num = bot.getUpdates(bot.last_message_received + 1);
    if (num > 0) handleNewMessages(num);
  }

  static unsigned long lastOLED = 0;
  if (millis() - lastOLED > 500) {
    lastOLED = millis();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Team 08");
    u8g2.drawLine(0, 12, 128, 12);
    u8g2.drawStr(0, 24, "T:"); u8g2.setCursor(16, 24); u8g2.print(temperature, 1);
    u8g2.drawStr(65, 24, "H:"); u8g2.setCursor(81, 24); u8g2.print(humidity, 1);
    u8g2.drawStr(0, 36, "Gas:"); u8g2.setCursor(30, 36); u8g2.print(gasValue);
    u8g2.drawStr(70, 36, "PIR:"); u8g2.drawStr(100, 36, pirState ? "Y" : "N");
    u8g2.drawStr(0, 48, "LED:"); u8g2.drawStr(30, 48, ledState ? "ON" : "OFF");
    u8g2.drawStr(0, 60, "Up:"); u8g2.setCursor(24, 60); u8g2.print(uptime);
    u8g2.sendBuffer();
  }
}

// ===== BLYNK CALLBACKS =====
BLYNK_WRITE(V1) {
  ledState = param.asInt();
  digitalWrite(PIN_LED, ledState ? HIGH : LOW);
  Serial.println("[Blynk] LED = " + String(ledState ? "ON" : "OFF"));
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1);
  Blynk.virtualWrite(V5, "Thái, Long, Đạt, Phúc, Linh");
  Serial.println("[Blynk] Connected & synced");
}