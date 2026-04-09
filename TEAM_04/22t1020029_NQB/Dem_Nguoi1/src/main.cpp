#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

//================= CHAN KET NOI =================
#define IR1_PIN 4
#define IR2_PIN 5
#define LED_PIN 2

//================= WIFI =========================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

//================= TELEGRAM =====================
#define BOT_TOKEN "8270443543:AAGNMrYrG1cARaeTiWUK6EdSMeVy0ISEe6o"
#define CHAT_ID "-5277488606"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

//================= LCD ==========================
LiquidCrystal_I2C lcd(0x27, 16, 2);

//================= BIEN HE THONG ================
int peopleCount = 0;

// 0 = chua co cam bien nao kich truoc
// 1 = IR1 kich truoc
// 2 = IR2 kich truoc
int firstSensor = 0;
unsigned long firstTriggerTime = 0;

// Chong lap xung
unsigned long lastIR1Trigger = 0;
unsigned long lastIR2Trigger = 0;

// Thoi gian khoa tranh 1 lan bam bi dem nhieu lan
const unsigned long debounceTime = 800;

// Thoi gian toi da giua 2 cam bien de tinh la 1 luot di qua
const unsigned long sequenceWindow = 2000;

//================= HAM PHU ======================
void connectWiFi() {
  Serial.print("Dang ket noi WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi da ket noi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Don gian nhat de test Telegram tren Wokwi / ESP32
  client.setInsecure();
}

void sendTelegramMessage(String message) {
  bool ok = bot.sendMessage(CHAT_ID, message, "");
  if (ok) {
    Serial.println("Gui Telegram thanh cong");
  } else {
    Serial.println("Gui Telegram that bai");
  }
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("People Counter");
  lcd.setCursor(0, 1);
  lcd.print("Count: ");
  lcd.print(peopleCount);
}

void printStatus(const char* msg) {
  Serial.println("--------------------------------");
  Serial.println(msg);
  Serial.print("So nguoi hien tai: ");
  Serial.println(peopleCount);
  Serial.println("--------------------------------");
}

void handleIR1() {
  unsigned long now = millis();

  // Chong lap
  if (now - lastIR1Trigger < debounceTime) return;
  lastIR1Trigger = now;

  Serial.println("IR1 duoc kich hoat");

  if (firstSensor == 0) {
    firstSensor = 1;
    firstTriggerTime = now;
    Serial.println("Dang cho IR2 de xac dinh huong di...");
    return;
  }

  // Neu IR2 da kich truoc do -> nguoi di ra
  if (firstSensor == 2 && (now - firstTriggerTime <= sequenceWindow)) {
    if (peopleCount > 0) {
      peopleCount--;
    }

    updateLCD();
    printStatus("=> PHAT HIEN NGUOI DI RA");

    sendTelegramMessage(
      "Co 1 nguoi di ra.\nSo nguoi hien tai: " + String(peopleCount)
    );

    firstSensor = 0;
    firstTriggerTime = 0;
    return;
  }

  // Neu chuoi cu khong hop le thi reset va bat dau lai tu IR1
  firstSensor = 1;
  firstTriggerTime = now;
  Serial.println("Reset chuoi, bat dau lai tu IR1");
}

void handleIR2() {
  unsigned long now = millis();

  // Chong lap
  if (now - lastIR2Trigger < debounceTime) return;
  lastIR2Trigger = now;

  Serial.println("IR2 duoc kich hoat");

  if (firstSensor == 0) {
    firstSensor = 2;
    firstTriggerTime = now;
    Serial.println("Dang cho IR1 de xac dinh huong di...");
    return;
  }

  // Neu IR1 da kich truoc do -> nguoi di vao
  if (firstSensor == 1 && (now - firstTriggerTime <= sequenceWindow)) {
    peopleCount++;

    updateLCD();
    printStatus("=> PHAT HIEN NGUOI DI VAO");

    sendTelegramMessage(
      "Co 1 nguoi di vao.\nSo nguoi hien tai: " + String(peopleCount)
    );

    firstSensor = 0;
    firstTriggerTime = 0;
    return;
  }

  // Neu chuoi cu khong hop le thi reset va bat dau lai tu IR2
  firstSensor = 2;
  firstTriggerTime = now;
  Serial.println("Reset chuoi, bat dau lai tu IR2");
}

void setup() {
  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  updateLCD();

  connectWiFi();
  sendTelegramMessage("He thong dem nguoi ESP32 da bat dau.");

  Serial.println("=== HE THONG DEM NGUOI BAT DAU ===");
  Serial.println("Cach test:");
  Serial.println("- Bam Send o IR1 roi bam Send o IR2 => Nguoi di vao");
  Serial.println("- Bam Send o IR2 roi bam Send o IR1 => Nguoi di ra");
  Serial.println();
}

void loop() {
  int ir1 = digitalRead(IR1_PIN);
  int ir2 = digitalRead(IR2_PIN);

  // IR receiver trong Wokwi thuong active LOW
  if (ir1 == LOW) {
    handleIR1();
    delay(50);
  }

  if (ir2 == LOW) {
    handleIR2();
    delay(50);
  }

  // Neu cho qua lau ma khong co cam bien thu hai thi reset
  if (firstSensor != 0 && (millis() - firstTriggerTime > sequenceWindow)) {
    Serial.println("Het thoi gian cho, reset chuoi dem.");
    firstSensor = 0;
    firstTriggerTime = 0;
  }

  // LED sang khi co nguoi
  digitalWrite(LED_PIN, peopleCount > 0 ? HIGH : LOW);
}