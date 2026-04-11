#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// =========================
// WIFI CONFIG
// =========================
#define USE_WOKWI_WIFI true

const char* WIFI_SSID = USE_WOKWI_WIFI ? "Wokwi-GUEST" : "Toi la Nobita";
const char* WIFI_PASSWORD = USE_WOKWI_WIFI ? "" : "MatKhauWiFi";
const int WIFI_CHANNEL = 6;

// =========================
// TELEGRAM CONFIG
// =========================
const char* BOT_TOKEN = "7572820121:AAGaUNuQz_-QeOI4SJqdptBvlZlQVOOdykk";
const char* CHAT_ID = "-1003990872264";

// =========================
// HARDWARE PINS
// =========================
#define SS_PIN 5
#define RST_PIN 22
#define SERVO_PIN 13
#define LED_GREEN 2
#define LED_RED 4
#define BUZZER_PIN 27

// =========================
// LOCK CONFIG
// =========================
const int LOCK_ANGLE = 0;
const int UNLOCK_ANGLE = 90;
const unsigned long DOOR_OPEN_TIME_MS = 5000;
const unsigned long AUTH_TIMEOUT_MS = 30000;
const unsigned long RFID_COOLDOWN_MS = 1200;
const unsigned long TELEGRAM_POLL_MS = 2000;

const String ACCESS_PIN = "123456";

// =========================
// AUTHORIZED RFID UID
// =========================
String allowedCards[] = {
  "01 02 03 04",
  "11 22 33 44",
  "55 66 77 88"
};

const int allowedCount = sizeof(allowedCards) / sizeof(allowedCards[0]);

// =========================
// OBJECTS
// =========================
MFRC522 rfid(SS_PIN, RST_PIN);
Servo doorServo;
WebServer server(80);
WiFiClientSecure secureClient;
UniversalTelegramBot bot(BOT_TOKEN, secureClient);

// =========================
// GLOBAL STATE
// =========================
bool isDoorOpen = false;
unsigned long doorOpenedAt = 0;

bool pendingWifiAuth = false;
unsigned long authStartedAt = 0;
String currentUid = "";
unsigned long lastCardReadAt = 0;
unsigned long lastTelegramPoll = 0;

// =========================
// HELPER
// =========================
String getUIDString(MFRC522::Uid* uid) {
  String result = "";
  for (byte i = 0; i < uid->size; i++) {
    if (uid->uidByte[i] < 0x10) result += "0";
    result += String(uid->uidByte[i], HEX);
    if (i < uid->size - 1) result += " ";
  }
  result.toUpperCase();
  return result;
}

bool isAuthorizedCard(const String& uid) {
  for (int i = 0; i < allowedCount; i++) {
    if (uid == allowedCards[i]) return true;
  }
  return false;
}

bool isAuthSessionValid() {
  return pendingWifiAuth && (millis() - authStartedAt <= AUTH_TIMEOUT_MS);
}

void clearPendingAuth(bool turnOffRedLed = true) {
  pendingWifiAuth = false;
  currentUid = "";
  if (turnOffRedLed) {
    digitalWrite(LED_RED, LOW);
  }
}

String getSystemStatus() {
  String s;
  s += "Trang thai cua: ";
  s += (isDoorOpen ? "DANG MO" : "DANG DONG");
  s += "\nWiFi: ";
  s += (WiFi.status() == WL_CONNECTED ? "DA KET NOI" : "MAT KET NOI");
  s += "\nIP: ";
  s += WiFi.localIP().toString();
  s += "\nCho xac thuc WiFi: ";
  s += (isAuthSessionValid() ? "CO" : "KHONG");
  if (isAuthSessionValid()) {
    s += "\nUID dang cho: ";
    s += currentUid;
  }
  return s;
}

void beepOK() {
  tone(BUZZER_PIN, 2000, 150);
}

void beepWait() {
  tone(BUZZER_PIN, 1600, 400);
}

void beepError() {
  tone(BUZZER_PIN, 1000, 200);
  delay(250);
  tone(BUZZER_PIN, 800, 300);
  delay(350);
}

void sendTelegramMessage(const String& msg) {
  if (WiFi.status() != WL_CONNECTED) return;
  bot.sendMessage(CHAT_ID, msg, "");
}

void lockDoor() {
  doorServo.write(LOCK_ANGLE);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  noTone(BUZZER_PIN);
  isDoorOpen = false;
  Serial.println("Door locked");
}

void unlockDoor(const String& reason) {
  doorServo.write(UNLOCK_ANGLE);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, LOW);
  beepOK();

  isDoorOpen = true;
  doorOpenedAt = millis();

  clearPendingAuth(false);

  Serial.println("Access granted - Door unlocked");
  sendTelegramMessage("Mo khoa thanh cong\nLy do: " + reason);
}

void denyAccess(const String& reason) {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);
  beepError();

  Serial.println("Access denied: " + reason);
  sendTelegramMessage("Truy cap bi tu choi\nLy do: " + reason);
}

void startWifiAuth(const String& uid) {
  pendingWifiAuth = true;
  authStartedAt = millis();
  currentUid = uid;

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);
  beepWait();

  String ip = WiFi.localIP().toString();

  Serial.println("RFID hop le. Dang cho xac thuc WiFi...");
  Serial.print("UID dang cho: ");
  Serial.println(uid);
  Serial.print("Mo trinh duyet den: http://");
  Serial.println(ip);

  String msg = "The RFID hop le: " + uid +
               "\nDang cho xac thuc WiFi/PIN trong 30 giay." +
               "\nIP ESP32: http://" + ip;
  sendTelegramMessage(msg);
}

void connectWiFi() {
  Serial.print("Dang ket noi WiFi");
  WiFi.mode(WIFI_STA);

  if (USE_WOKWI_WIFI) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  } else {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

String htmlPage() {
  String html;
  html += "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP32 Smart Lock</title>";
  html += "<style>";
  html += "body{font-family:Arial,sans-serif;background:#f5f7fb;margin:0;padding:24px;color:#222;}";
  html += ".box{max-width:520px;margin:auto;background:#fff;padding:24px;border-radius:16px;box-shadow:0 10px 30px rgba(0,0,0,.08);}";
  html += "h1{margin-top:0;font-size:24px;}";
  html += ".ok{color:#0a8f3d;font-weight:bold;}";
  html += ".warn{color:#b86a00;font-weight:bold;}";
  html += ".bad{color:#c62828;font-weight:bold;}";
  html += "input{width:100%;padding:12px;font-size:16px;border:1px solid #ccc;border-radius:10px;box-sizing:border-box;}";
  html += "button{margin-top:12px;width:100%;padding:12px;background:#0b5ed7;color:#fff;border:none;border-radius:10px;font-size:16px;cursor:pointer;}";
  html += "code{background:#f1f3f5;padding:2px 6px;border-radius:6px;}";
  html += "</style></head><body><div class='box'>";
  html += "<h1>ESP32 Smart Door Lock</h1>";
  html += "<p>WiFi: <span class='ok'>";
  html += (WiFi.status() == WL_CONNECTED ? "Da ket noi" : "Mat ket noi");
  html += "</span></p>";
  html += "<p>IP: <code>";
  html += WiFi.localIP().toString();
  html += "</code></p>";
  html += "<p>Cua hien tai: <strong>";
  html += (isDoorOpen ? "Dang mo" : "Dang dong");
  html += "</strong></p>";

  if (isAuthSessionValid()) {
    unsigned long remain = (AUTH_TIMEOUT_MS - (millis() - authStartedAt)) / 1000;
    html += "<p class='warn'>Da quet the hop le. Hay nhap PIN trong ";
    html += String(remain);
    html += " giay.</p>";
    html += "<p>UID: <code>" + currentUid + "</code></p>";
    html += "<form action='/verify' method='POST'>";
    html += "<input type='password' name='pin' placeholder='Nhap ma PIN'>";
    html += "<button type='submit'>Xac thuc mo khoa</button>";
    html += "</form>";
  } else {
    html += "<p class='bad'>Chua co the hop le nao dang cho xac thuc.</p>";
    html += "<p>Hay quet the RFID truoc.</p>";
  }

  html += "<hr>";
  html += "<p>Telegram commands: <code>/status</code>, <code>/unlock</code>, <code>/lock</code>, <code>/approve</code></p>";
  html += "</div></body></html>";
  return html;
}

// =========================
// WEB HANDLERS
// =========================
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void handleVerify() {
  if (!pendingWifiAuth) {
    server.send(403, "text/html", "<h2>Khong co phien xac thuc nao dang cho.</h2><a href='/'>Quay lai</a>");
    return;
  }

  if (!isAuthSessionValid()) {
    clearPendingAuth();
    noTone(BUZZER_PIN);
    server.send(408, "text/html", "<h2>Het thoi gian xac thuc.</h2><a href='/'>Quay lai</a>");
    return;
  }

  String pin = server.arg("pin");
  if (pin == ACCESS_PIN) {
    String uid = currentUid;
    unlockDoor("RFID + PIN web hop le\nUID: " + uid);
    server.send(200, "text/html", "<h2>Mo khoa thanh cong.</h2><a href='/'>Quay lai</a>");
  } else {
    String uid = currentUid;
    denyAccess("Sai ma PIN web cho UID: " + uid);
    clearPendingAuth();
    noTone(BUZZER_PIN);
    server.send(401, "text/html", "<h2>Sai PIN. Tu choi truy cap.</h2><a href='/'>Quay lai</a>");
  }
}

void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/verify", HTTP_POST, handleVerify);
  server.begin();

  Serial.println("Web server started");
  Serial.print("Truy cap: http://");
  Serial.println(WiFi.localIP());
}

// =========================
// TELEGRAM
// =========================
void handleTelegramMessages(int count) {
  for (int i = 0; i < count; i++) {
    String chatId = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String fromName = bot.messages[i].from_name;

    if (chatId != CHAT_ID) {
      bot.sendMessage(chatId, "Unauthorized user", "");
      continue;
    }

    Serial.print("Telegram message from ");
    Serial.print(fromName);
    Serial.print(": ");
    Serial.println(text);

    if (text == "/start") {
      String welcome = "ESP32 Smart Lock\n";
      welcome += "/status - xem trang thai\n";
      welcome += "/unlock - mo cua tu xa\n";
      welcome += "/lock - dong cua\n";
      welcome += "/approve - phe duyet neu dang cho xac thuc";
      bot.sendMessage(CHAT_ID, welcome, "");
    } else if (text == "/status") {
      bot.sendMessage(CHAT_ID, getSystemStatus(), "");
    } else if (text == "/lock") {
      lockDoor();
      bot.sendMessage(CHAT_ID, "Da dong cua.", "");
    } else if (text == "/unlock") {
      unlockDoor("Lenh Telegram /unlock");
      bot.sendMessage(CHAT_ID, "Da mo cua bang Telegram.", "");
    } else if (text == "/approve") {
      if (isAuthSessionValid()) {
        String uid = currentUid;
        unlockDoor("RFID + Telegram /approve\nUID: " + uid);
        bot.sendMessage(CHAT_ID, "Da phe duyet va mo cua.", "");
      } else {
        bot.sendMessage(CHAT_ID, "Khong co phien nao dang cho xac thuc.", "");
      }
    } else {
      bot.sendMessage(CHAT_ID, "Lenh khong hop le. Dung /start de xem menu.", "");
    }
  }
}

void pollTelegram() {
  if (millis() - lastTelegramPoll < TELEGRAM_POLL_MS) return;
  lastTelegramPoll = millis();

  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages) {
    handleTelegramMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

// =========================
// RFID
// =========================
void handleRFID() {
  if (millis() - lastCardReadAt < RFID_COOLDOWN_MS) return;
  if (pendingWifiAuth) return;
  if (!rfid.PICC_IsNewCardPresent()) return;
  Serial.println("Card present detected");
  if (!rfid.PICC_ReadCardSerial()) {
    Serial.println("Card present but UID read failed");
    return;
  }
  lastCardReadAt = millis();

  String uid = getUIDString(&rfid.uid);
  Serial.print("Card detected. UID: ");
  Serial.println(uid);

  if (isAuthorizedCard(uid)) {
    startWifiAuth(uid);
  } else {
    denyAccess("The RFID khong hop le: " + uid);
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// =========================
// SETUP
// =========================
void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  noTone(BUZZER_PIN);

  doorServo.attach(SERVO_PIN);
  lockDoor();

  SPI.begin(18, 19, 23, 5);
  rfid.PCD_Init();

  connectWiFi();
  secureClient.setInsecure();
  setupWebServer();

  sendTelegramMessage("ESP32 Smart Lock da khoi dong.\nIP: http://" + WiFi.localIP().toString());

  Serial.println("====================================");
  Serial.println("ESP32 SMART DOOR LOCK READY");
  Serial.println("1. Quet the RFID");
  Serial.println("2. Dung Telegram /approve de xac thuc");
  Serial.println("====================================");
}

// =========================
// LOOP
// =========================
void loop() {
  server.handleClient();
  handleRFID();
  pollTelegram();

  if (pendingWifiAuth && !isAuthSessionValid()) {
    String expiredUid = currentUid;
    clearPendingAuth();
    noTone(BUZZER_PIN);
    sendTelegramMessage("Het han xac thuc cho UID: " + expiredUid);
    Serial.println("Authentication timeout");
  }

  if (isDoorOpen && millis() - doorOpenedAt >= DOOR_OPEN_TIME_MS) {
    lockDoor();
    sendTelegramMessage("Cua da tu dong dong lai.");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi mat ket noi. Dang thu ket noi lai...");
    connectWiFi();
    setupWebServer();
  }

  delay(10);
}
