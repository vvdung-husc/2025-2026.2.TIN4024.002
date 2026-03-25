/*
<<<<<<< HEAD
  THÔNG TIN NHÓM 09
  1. Nguyễn Đăng Khôi
  2. Nguyễn Gia Huy
  3. Nguyễn Lê Bá Minh  
  4. Nguyễn Nhật Hào

  */
=======
THONG TIN NHOM 04
1. Nguyen Dang Khôi
2. Nguyễn Gia Huy - Telegram: @quy691907
*/
>>>>>>> df6496a (giahuy)

#define BLYNK_TEMPLATE_ID "TMPL6wHdZ2xFq"
#define BLYNK_TEMPLATE_NAME "ESP8266BLYNKTELEGRAM"
#define BLYNK_AUTH_TOKEN "-8LazYUeNX8TFLW3VwOFdG5fO5ILlHen"

#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp8266.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// ===================== WIFI =====================
char ssid[] = "Quy";
char pass[] = "19072004";

// ===================== TELEGRAM =====================
#define BOT_TOKEN "8657448928:AAEQ9ThAa3rgeKJhClgUSaGbxWemT-5fXic"
#define ADMIN_CHAT_ID  "-1003726763093"

// ===================== PIN CONFIG =====================
#define DHTPIN      D3
#define DHTTYPE     DHT22
#define RELAY_PIN   2
#define MQ2_PIN     A0
#define OLED_SDA    D2
#define OLED_SCL    D1

// ===================== TEAM INFO =====================
const char* TEAM_NAME = "Team 09_Nhom02";

// ===================== GAS THRESHOLD =====================
const int GAS_MEDIUM_THRESHOLD = 400;
const int GAS_HIGH_THRESHOLD   = 700;

// ===================== OBJECTS =====================
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SH1106G display(128, 64, &Wire, -1);
BlynkTimer timer;
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// ===================== GLOBAL VARIABLES =====================
float temperature = 0.0f;
float humidity = 0.0f;
int gasValue = 0;
bool relayState = false;
bool useRandomGas = false;

unsigned long lastTelegramCheck = 0;
unsigned long lastGasNotify = 0;
unsigned long lastWeatherNotify = 0;
unsigned long lastDisplayUpdate = 0;

float lastSentTemp = -999.0f;
float lastSentHum  = -999.0f;
int lastSentGas    = -1;

// ===================== HELPER =====================
String formatUptime(unsigned long ms) {
  unsigned long totalSec = ms / 1000;
  unsigned long days = totalSec / 86400;
  unsigned long hours = (totalSec % 86400) / 3600;
  unsigned long mins = (totalSec % 3600) / 60;
  unsigned long secs = totalSec % 60;

  char buf[32];
  snprintf(buf, sizeof(buf), "%luD %02lu:%02lu:%02lu", days, hours, mins, secs);
  return String(buf);
}

String formatUptimeShort(unsigned long ms) {
  unsigned long totalSec = ms / 1000;
  unsigned long hours = (totalSec % 86400) / 3600;
  unsigned long mins = (totalSec % 3600) / 60;
  unsigned long secs = totalSec % 60;

  char buf[16];
  snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu", hours, mins, secs);
  return String(buf);
}

bool isCommand(const String& text, const String& cmd) {
  return (text == cmd) || text.startsWith(cmd + "@");
}

void setRelay(bool state) {
  relayState = state;

  // Relay active LOW
  digitalWrite(RELAY_PIN, relayState ? LOW : HIGH);

  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, relayState ? 1 : 0);
  }
}

bool sendTelegramTo(const String& chat_id, const String& text) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[TG] Send fail: WiFi not connected");
    return false;
  }

  bool ok = bot.sendMessage(chat_id, text, "");
  Serial.print("[TG] Send to ");
  Serial.print(chat_id);
  Serial.print(" -> ");
  Serial.println(ok ? "OK" : "FAIL");
  return ok;
}

bool sendTelegramAdmin(const String& text) {
  return sendTelegramTo(ADMIN_CHAT_ID, text);
}

// ===================== TELEGRAM TEXT =====================
String getFormattedSensorData() {
  String msg;
  msg += "BAO CAO CAM BIEN\n";
  msg += "----------------------\n";
  msg += "Nhiet do: " + String(temperature, 1) + " C\n";
  msg += "Do am   : " + String(humidity, 1) + " %\n";
  msg += "Khi gas : " + String(gasValue) + "\n";
  msg += "Den     : " + String(relayState ? "BAT" : "TAT") + "\n";
  msg += "Uptime  : " + formatUptime(millis()) + "\n";
  msg += "----------------------\n";
  msg += TEAM_NAME;
  return msg;
}

String getCommandHelpText(const String& from_name) {
  String name = from_name;
  if (name.length() == 0) name = "ban";

  String help;
  help += "Xin chao " + name + "\n";
  help += "ESP8266 CONTROL BOT\n";
  help += "====================\n";
  help += "Lenh dieu khien:\n";
  help += "/led_on      - Bat den\n";
  help += "/led_off     - Tat den\n";
  help += "/led_status  - Xem trang thai den\n";
  help += "\n";
  help += "Lenh cam bien:\n";
  help += "/get_weather - Nhiet do va do am\n";
  help += "/get_gas     - Gia tri khi gas\n";
  help += "/get_all     - Xem tat ca du lieu\n";
  help += "\n";
  help += "He thong:\n";
  help += "/start       - Menu chinh\n";
  help += "/uptime      - Thoi gian hoat dong\n";
  help += "====================\n";
  help += TEAM_NAME;
  return help;
}

// ===================== NOTIFICATIONS =====================
void notifyWeatherChange() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (millis() - lastWeatherNotify < 300000UL) return;  // 5 phut

  bool tempChanged = fabs(temperature - lastSentTemp) >= 0.5f;
  bool humChanged  = fabs(humidity - lastSentHum) >= 1.0f;

  if (tempChanged || humChanged) {
    String alert;
    alert += "THAY DOI MOI TRUONG\n";
    alert += "----------------------\n";

    if (tempChanged) {
      alert += "Nhiet do: " + String(temperature, 1) + " C\n";
    }
    if (humChanged) {
      alert += "Do am   : " + String(humidity, 1) + " %\n";
    }

    alert += "----------------------\n";
    alert += TEAM_NAME;

    if (sendTelegramAdmin(alert)) {
      lastSentTemp = temperature;
      lastSentHum = humidity;
      lastWeatherNotify = millis();
    }
  }
}

void notifyGasLevel() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (millis() - lastGasNotify < 300000UL) return;  // 5 phut

  if (lastSentGas < 0 || abs(gasValue - lastSentGas) >= 50) {
    String alert;

    if (gasValue > GAS_HIGH_THRESHOLD) {
      alert += "CANH BAO KHI GAS CAO!\n";
      alert += "----------------------\n";
      alert += "Gia tri: " + String(gasValue) + "\n";
      alert += "Vui long kiem tra ngay!\n";
    } else if (gasValue > GAS_MEDIUM_THRESHOLD) {
      alert += "KHI GAS TANG\n";
      alert += "----------------------\n";
      alert += "Gia tri: " + String(gasValue) + "\n";
      alert += "Dang theo doi...\n";
    } else {
      alert += "CAP NHAT KHI GAS\n";
      alert += "----------------------\n";
      alert += "Gia tri: " + String(gasValue) + "\n";
      alert += "Muc binh thuong\n";
    }

    alert += "----------------------\n";
    alert += TEAM_NAME;

    if (sendTelegramAdmin(alert)) {
      lastSentGas = gasValue;
      lastGasNotify = millis();
    }
  }
}

// ===================== SENSOR =====================
int readGasAverage() {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(MQ2_PIN);
    delay(5);
  }
  return (int)(sum / 10);
}

void readSensors() {
  static unsigned long lastRead = 0;
  if (millis() - lastRead < 2200UL) return;
  lastRead = millis();

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t)) {
    temperature = t;
  } else {
    Serial.println("[DHT] Loi doc nhiet do!");
  }

  if (!isnan(h)) {
    humidity = h;
  } else {
    Serial.println("[DHT] Loi doc do am!");
  }

  gasValue = useRandomGas ? random(200, 900) : readGasAverage();
}

void printSerialData() {
  Serial.println("=================================");
  Serial.printf("Uptime    : %s\n", formatUptime(millis()).c_str());
  Serial.printf("Nhiet do  : %.1f C\n", temperature);
  Serial.printf("Do am     : %.1f %%\n", humidity);
  Serial.printf("Khi gas   : %d\n", gasValue);
  Serial.printf("Relay     : %s\n", relayState ? "BAT" : "TAT");
  Serial.printf("WiFi      : %s\n", WiFi.status() == WL_CONNECTED ? "OK" : "LOST");
  Serial.printf("Blynk     : %s\n", Blynk.connected() ? "OK" : "LOST");
}

// ===================== OLED =====================
void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  display.setCursor(0, 0);
  display.println("ESP8266 IoT Team 09");

  display.setCursor(0, 10);
  display.printf("Temp: %.1f C", temperature);

  display.setCursor(0, 20);
  display.printf("Hum : %.1f %%", humidity);

  display.setCursor(0, 30);
  display.printf("Gas : %d", gasValue);

  display.setCursor(0, 40);
  display.printf("Lamp: %s", relayState ? "ON" : "OFF");

  display.setCursor(0, 50);
  display.print("Up  : ");
  display.print(formatUptimeShort(millis()));

  display.display();
}

// ===================== BLYNK =====================
BLYNK_CONNECTED() {
  Serial.println("[BLYNK] Connected");
  Blynk.syncVirtual(V0);
}

BLYNK_WRITE(V0) {
  int value = param.asInt();
  setRelay(value == 1);
}

void updateBlynk() {
  if (!Blynk.connected()) return;

  Blynk.virtualWrite(V0, relayState ? 1 : 0);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  Blynk.virtualWrite(V3, gasValue);
  Blynk.virtualWrite(V4, formatUptime(millis()));
  Blynk.virtualWrite(V5, TEAM_NAME);
}

// ===================== TELEGRAM =====================
void handleNewMessages(int numNewMessages) {
  Serial.print("[TG] handleNewMessages = ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id   = bot.messages[i].chat_id;
    String text      = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    text.trim();

    Serial.println("---------------------------------");
    Serial.print("[TG] Chat ID   : ");
    Serial.println(chat_id);
    Serial.print("[TG] From name : ");
    Serial.println(from_name);
    Serial.print("[TG] Text      : ");
    Serial.println(text);

    if (text.length() == 0) {
      Serial.println("[TG] Empty text -> bo qua");
      continue;
    }

    if (text == "/start" || text == "/start@Team9_002_esp8266_bot") {
      sendTelegramTo(chat_id, getCommandHelpText(from_name));
    }
    else if (text == "/led_on" || text == "/led_on@Team9_002_esp8266_bot") {
      setRelay(true);
      sendTelegramTo(chat_id, "Den da BAT\n" + String(TEAM_NAME));
    }
    else if (text == "/led_off" || text == "/led_off@Team9_002_esp8266_bot") {
      setRelay(false);
      sendTelegramTo(chat_id, "Den da TAT\n" + String(TEAM_NAME));
    }
    else if (text == "/led_status" || text == "/led_status@Team9_002_esp8266_bot") {
      String status = "Trang thai den: ";
      status += (relayState ? "BAT" : "TAT");
      status += "\n";
      status += TEAM_NAME;
      sendTelegramTo(chat_id, status);
    }
    else if (text == "/get_weather" || text == "/get_weather@Team9_002_esp8266_bot") {
      String weather;
      weather += "THOI TIET HIEN TAI\n";
      weather += "----------------------\n";
      weather += "Nhiet do: " + String(temperature, 1) + " C\n";
      weather += "Do am   : " + String(humidity, 1) + " %\n";
      weather += "----------------------\n";
      weather += TEAM_NAME;
      sendTelegramTo(chat_id, weather);
    }
    else if (text == "/get_gas" || text == "/get_gas@Team9_002_esp8266_bot") {
      String gasMsg;
      gasMsg += "KHI GAS\n";
      gasMsg += "----------------------\n";
      gasMsg += "Gia tri: " + String(gasValue) + "\n";
      if (gasValue > GAS_HIGH_THRESHOLD) gasMsg += "MUC CAO!\n";
      else if (gasValue > GAS_MEDIUM_THRESHOLD) gasMsg += "MUC TRUNG BINH\n";
      else gasMsg += "MUC BINH THUONG\n";
      gasMsg += "----------------------\n";
      gasMsg += TEAM_NAME;
      sendTelegramTo(chat_id, gasMsg);
    }
    else if (text == "/get_all" || text == "/get_all@Team9_002_esp8266_bot") {
      sendTelegramTo(chat_id, getFormattedSensorData());
    }
    else if (text == "/uptime" || text == "/uptime@Team9_002_esp8266_bot") {
      String uptime;
      uptime += "THOI GIAN HOAT DONG\n";
      uptime += "----------------------\n";
      uptime += formatUptime(millis()) + "\n";
      uptime += "----------------------\n";
      uptime += TEAM_NAME;
      sendTelegramTo(chat_id, uptime);
    }
    else {
      sendTelegramTo(chat_id, "Lenh khong hop le!\nGo /start de xem danh sach lenh.");
    }
  }
}

void checkTelegram() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[TG] WiFi not connected");
    return;
  }

  if (millis() - lastTelegramCheck < 2000UL) return;
  lastTelegramCheck = millis();

  Serial.println("=== CHECK TELEGRAM ===");

  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  Serial.print("[TG] last_message_received = ");
  Serial.println(bot.last_message_received);

  Serial.print("[TG] numNewMessages = ");
  Serial.println(numNewMessages);

  if (numNewMessages < 0) {
    Serial.println("[TG] Loi getUpdates");
    return;
  }

  while (numNewMessages > 0) {
    handleNewMessages(numNewMessages);

    numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    Serial.print("[TG] next numNewMessages = ");
    Serial.println(numNewMessages);
  }
}

// ===================== TASKS =====================
void taskReadData() {
  readSensors();
  printSerialData();
}

void taskCloudUpdate() {
  updateBlynk();
}

void taskTelegramNotify() {
  notifyWeatherChange();
  notifyGasLevel();
}

// ===================== WIFI =====================
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  Serial.print("[WIFI] Dang ket noi");
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000UL) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[WIFI] OK. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[WIFI] Khong ket noi duoc.");
  }
}

void reconnectIfNeeded() {
  static unsigned long lastReconnectAttempt = 0;
  if (millis() - lastReconnectAttempt < 5000UL) return;
  lastReconnectAttempt = millis();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WIFI] Mat ket noi, dang thu lai...");
    WiFi.disconnect();
    delay(200);
    WiFi.begin(ssid, pass);
    return;
  }

  if (!Blynk.connected()) {
    Serial.println("[BLYNK] Dang ket noi lai...");
    Blynk.connect(1000);
  }
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("BOOT START");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  dht.begin();

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(0x3C, true)) {
    Serial.println("[OLED] Khong tim thay OLED SH1106!");
    while (true) {
      delay(1000);
    }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Dang khoi dong...");
  display.display();

  randomSeed(analogRead(A0));

  secured_client.setInsecure();
  secured_client.setTimeout(10000);
  bot.longPoll = 0;

  connectWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[TG] Flush update cu...");

    bot.last_message_received = 522869480;

    int n = bot.getUpdates(bot.last_message_received + 1);

    Serial.print("[TG] Flush xong, n = ");
    Serial.println(n);

    Serial.print("[TG] last_message_received = ");
    Serial.println(bot.last_message_received);
  }
  Blynk.config(BLYNK_AUTH_TOKEN);
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.connect(3000);
  }

  setRelay(false);
  readSensors();
  updateOLED();
  updateBlynk();

  timer.setInterval(2500L, taskReadData);
  timer.setInterval(3000L, taskCloudUpdate);
  timer.setInterval(5000L, taskTelegramNotify);

  Serial.println("He thong san sang.");

  if (WiFi.status() == WL_CONNECTED) {
    String msg;
    msg += "ESP8266 DA KHOI DONG!\n";
    msg += "----------------------\n";
    msg += "IP: " + WiFi.localIP().toString() + "\n";
    msg += "WiFi: " + String(ssid) + "\n";
    msg += "Temp: " + String(temperature, 1) + " C\n";
    msg += "Relay: " + String(relayState ? "BAT" : "TAT") + "\n";
    msg += "----------------------\n";
    msg += "San sang nhan lenh!\n";
    msg += TEAM_NAME;
    sendTelegramAdmin(msg);
  }

  Serial.println("BOOT DONE");
}

// ===================== LOOP =====================
void loop() {
  if (millis() - lastDisplayUpdate >= 200UL) {
    lastDisplayUpdate = millis();
    updateOLED();
  }

  reconnectIfNeeded();

  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }

  timer.run();
  checkTelegram();
}