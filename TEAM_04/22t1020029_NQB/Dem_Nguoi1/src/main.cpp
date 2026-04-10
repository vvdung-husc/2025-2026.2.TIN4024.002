#define BLYNK_TEMPLATE_ID "TMPL6AeMwyWpp"
#define BLYNK_TEMPLATE_NAME "ESP32 Counter"
#define BLYNK_AUTH_TOKEN "ABrO2v1MRAwff_B4aSldlRj6htDCleUy"


#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <BlynkSimpleEsp32.h>

//================= WIFI =========================
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

//================= TELEGRAM =====================
#define BOT_TOKEN "8270443543:AAGNMrYrG1cARaeTiWUK6EdSMeVy0ISEe6o"
#define CHAT_ID "-5277488606"

WiFiClientSecure telegramClient;
UniversalTelegramBot bot(BOT_TOKEN, telegramClient);

//================= LCD ==========================
LiquidCrystal_I2C lcd(0x27, 16, 2);

//================= CHAN =========================
#define IR1_PIN 4
#define IR2_PIN 5
#define LED_PIN 2

//================= BIEN =========================
int peopleCount = 0;
int firstSensor = 0; // 0: chua co, 1: IR1 truoc, 2: IR2 truoc
unsigned long firstTriggerTime = 0;

unsigned long lastIR1Trigger = 0;
unsigned long lastIR2Trigger = 0;

const unsigned long debounceTime = 800;
const unsigned long sequenceWindow = 2000;

BlynkTimer timer;

//================= HAM PHU ======================
void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("People Counter");
  lcd.setCursor(0, 1);
  lcd.print("Count: ");
  lcd.print(peopleCount);
}

void updateLED() {
  digitalWrite(LED_PIN, peopleCount > 0 ? HIGH : LOW);
}

void sendBlynkStatus(const String &msg) {
  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, peopleCount);
    Blynk.virtualWrite(V1, msg);
  }
}

void sendTelegram(const String &msg) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Telegram: mat WiFi");
    return;
  }

  bool ok = bot.sendMessage(CHAT_ID, msg, "");
  Serial.println(ok ? "Telegram: gui thanh cong" : "Telegram: gui that bai");
}

void showStatus(const String &eventText) {
  Serial.println("--------------------------------");
  Serial.println(eventText);
  Serial.print("So nguoi hien tai: ");
  Serial.println(peopleCount);
  Serial.println("--------------------------------");
}

void connectWiFi() {
  Serial.print("Dang ket noi WiFi");
  // Wokwi khuyen dung channel 6 de bo qua scan, nhanh hon
  WiFi.begin(ssid, pass, 6);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(200);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi da ket noi");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi ket noi that bai");
  }
}

void connectBlynk() {
  Serial.println("Bat dau ket noi Blynk...");
  Blynk.config(BLYNK_AUTH_TOKEN);

  bool ok = Blynk.connect(10000); // timeout 10s
  if (ok) {
    Serial.println("Blynk da ket noi");
  } else {
    Serial.println("Blynk ket noi that bai");
  }
}

void reconnectCloud() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (!Blynk.connected() && WiFi.status() == WL_CONNECTED) {
    connectBlynk();
  }
}

//================= BLYNK ========================
BLYNK_CONNECTED() {
  Serial.println("BLYNK_CONNECTED duoc goi");
  Blynk.virtualWrite(V0, peopleCount);
  Blynk.virtualWrite(V1, "He thong ESP32 da khoi dong");
}

// Nut reset o V2
BLYNK_WRITE(V2) {
  int val = param.asInt();

  if (val == 1) {
    peopleCount = 0;
    firstSensor = 0;
    firstTriggerTime = 0;

    updateLCD();
    updateLED();

    sendBlynkStatus("He thong da reset ve 0");
    sendTelegram("He thong da reset ve 0");
    Serial.println("Reset tu Blynk thanh cong");

    // tra nut ve 0
    Blynk.virtualWrite(V2, 0);
  }
}

//================= IR1 ==========================
// IR2 truoc -> IR1 sau = DI RA
void handleIR1() {
  unsigned long now = millis();

  if (now - lastIR1Trigger < debounceTime) return;
  lastIR1Trigger = now;

  Serial.println("IR1 duoc kich hoat");

  if (firstSensor == 0) {
    firstSensor = 1;
    firstTriggerTime = now;
    Serial.println("Dang cho IR2...");
    return;
  }

  if (firstSensor == 2 && (now - firstTriggerTime <= sequenceWindow)) {
    if (peopleCount > 0) {
      peopleCount--;
    }

    updateLCD();
    updateLED();

    showStatus("PHAT HIEN NGUOI DI RA");
    sendBlynkStatus("Co 1 nguoi di ra");
    sendTelegram("Co 1 nguoi di ra\nSo nguoi hien tai: " + String(peopleCount));

    firstSensor = 0;
    firstTriggerTime = 0;
    return;
  }

  firstSensor = 1;
  firstTriggerTime = now;
  Serial.println("Reset chuoi, bat dau lai tu IR1");
}

//================= IR2 ==========================
// IR1 truoc -> IR2 sau = DI VAO
void handleIR2() {
  unsigned long now = millis();

  if (now - lastIR2Trigger < debounceTime) return;
  lastIR2Trigger = now;

  Serial.println("IR2 duoc kich hoat");

  if (firstSensor == 0) {
    firstSensor = 2;
    firstTriggerTime = now;
    Serial.println("Dang cho IR1...");
    return;
  }

  if (firstSensor == 1 && (now - firstTriggerTime <= sequenceWindow)) {
    peopleCount++;

    updateLCD();
    updateLED();

    showStatus("PHAT HIEN NGUOI DI VAO");
    sendBlynkStatus("Co 1 nguoi di vao");
    sendTelegram("Co 1 nguoi di vao\nSo nguoi hien tai: " + String(peopleCount));

    firstSensor = 0;
    firstTriggerTime = 0;
    return;
  }

  firstSensor = 2;
  firstTriggerTime = now;
  Serial.println("Reset chuoi, bat dau lai tu IR2");
}

//================= SETUP ========================
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  lcd.init();
  lcd.backlight();
  updateLCD();

  telegramClient.setInsecure();

  connectWiFi();
  if (WiFi.status() == WL_CONNECTED) {
    connectBlynk();
  }

  updateLCD();
  updateLED();

  sendTelegram("He thong ESP32 da khoi dong");
  sendBlynkStatus("He thong ESP32 da khoi dong");

  timer.setInterval(5000L, reconnectCloud);

  Serial.println("=== HE THONG DEM NGUOI BAT DAU ===");
  Serial.println("Test:");
  Serial.println("IR1 -> IR2 = nguoi di vao");
  Serial.println("IR2 -> IR1 = nguoi di ra");
}

//================= LOOP =========================
void loop() {
  Blynk.run();
  timer.run();

  int ir1 = digitalRead(IR1_PIN);
  int ir2 = digitalRead(IR2_PIN);

  // IR receiver trong Wokwi active LOW
  if (ir1 == LOW) {
    handleIR1();
    delay(50);
  }

  if (ir2 == LOW) {
    handleIR2();
    delay(50);
  }

  if (firstSensor != 0 && millis() - firstTriggerTime > sequenceWindow) {
    Serial.println("Het thoi gian cho, reset chuoi dem");
    firstSensor = 0;
    firstTriggerTime = 0;
  }
}