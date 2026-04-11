/*
THÔNG TIN NHÓM 7
1. Lê Công Mẫn
2. Võ Tiến Đạt - Telegram: Vo Dat
3. Hồ Thị Kiều Oanh
*/

// --- BLYNK INFO ---
#define BLYNK_TEMPLATE_ID "TMPL6AwRzGKE_"
#define BLYNK_TEMPLATE_NAME "ESMart"
#define BLYNK_AUTH_TOKEN "QwvoQYeTSqLPUCQ6U3vw-OpstiKTwGup"

// --- TELEGRAM INFO ---
#define BOT_TOKEN "8743975552:AAHKyIiv8Nvy9kzgJNEBDCkpz-v0bkl4y7A"
#define CHAT_ID "-5177244186" // Nhớ là có dấu trừ ở đầu nhé

#define BLYNK_PRINT Serial

#include <Arduino.h>      
#include <ESP8266WiFi.h>           // Đã đổi thành thư viện WiFi của ESP8266
#include <BlynkSimpleEsp8266.h>    // Đã đổi thành thư viện Blynk của ESP8266
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// --- THÔNG TIN WIFI CỦA WOKWI (KHÔNG ĐỔI) ---
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// --- 1. CẤU HÌNH CHÂN CHO NODE MCU ESP8266 ---
// Chân I2C cho Màn hình OLED
#define OLED_SDA 4 // Chân D2 trên NodeMCU
#define OLED_SCL 5 // Chân D1 trên NodeMCU
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Chân DHT22
#define DHTPIN 2   // Chân D4 trên NodeMCU
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

// Chân Đèn LED
#define LED_GREEN  14  // Chân D5 trên NodeMCU
#define LED_YELLOW 12  // Chân D6 trên NodeMCU
#define LED_RED    13  // Chân D7 trên NodeMCU

// Cảm biến MQ2 (ESP8266 CHỈ CÓ 1 CHÂN ANALOG LÀ A0)
#define MQ2PIN A0 

// --- BIẾN TOÀN CỤC ---
BlynkTimer timer;
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

int currentLedToBlink = -1;  // Lưu trạng thái đèn nào đang cần nháy
bool masterLedSwitch = true; // Trạng thái công tắc trên app Blynk (V1)
String currentStatus = "";   // Lưu trạng thái (HOT, COLD...)
float currentT = 0;
float currentH = 0;
int currentGasValue = 0;     // Biến lưu giá trị khí gas thật

// Các biến phục vụ Telegram
unsigned long bot_lasttime = 0; // Thời gian check tin nhắn
float lastTempReported = 0;     // Lưu nhiệt độ cũ để báo cáo khi có thay đổi

// Hàm hỗ trợ tắt hết đèn
void turnOffAllLeds() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
}

// Hàm xử lý nháy đèn KHÔNG DÙNG DELAY
void handleBlinking() {
  if (currentLedToBlink == -1 || !masterLedSwitch) {
    turnOffAllLeds();
    return;
  }

  static unsigned long previousMillis = 0;
  static bool ledState = LOW;
  
  if (millis() - previousMillis >= 200) {
    previousMillis = millis();
    ledState = !ledState;
    
    turnOffAllLeds(); 
    if (ledState) {
      digitalWrite(currentLedToBlink, HIGH); 
    }
  }
}

// Hàm cập nhật Màn hình OLED
void updateOLED() {
  display.clearDisplay();
  display.setCursor(0, 0); 
  display.setTextSize(1);
  display.print("NHOM 7: ");
  display.print(currentStatus); 
  display.setCursor(0, 16); 
  display.setTextSize(2);
  display.print(currentT); 
  display.setTextSize(1);
  display.cp437(true); 
  display.write(167); display.print("C"); 
  display.setCursor(0, 40);
  display.setTextSize(1);
  display.print("Humidity: ");
  display.print(currentH);
  display.print(" %");
  display.display();
}

// --- HÀM TELEGRAM: Xử lý tin nhắn đến ---
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    if (chat_id != CHAT_ID) continue; 
    
    String text = bot.messages[i].text;
    Serial.print("Đã nhận lệnh Telegram: ");
    Serial.println(text);
    
    if (text == "/led_on") {
      masterLedSwitch = true;
      Blynk.virtualWrite(V1, 1); 
      bot.sendMessage(chat_id, "Hệ thống đèn đã được BẬT 🟢", "");
    } 
    else if (text == "/led_off") {
      masterLedSwitch = false;
      Blynk.virtualWrite(V1, 0); 
      turnOffAllLeds();
      bot.sendMessage(chat_id, "Hệ thống đèn đã bị TẮT 🔴", "");
    } 
    else if (text == "/get_weather") {
      String msg = "📊 BÁO CÁO MÔI TRƯỜNG NHÓM 7:\n";
      msg += "🌡 Nhiệt độ: " + String(currentT) + "°C\n";
      msg += "💧 Độ ẩm: " + String(currentH) + "%\n";
      msg += "💨 Mức khí Gas: " + String(currentGasValue) + " (Raw)\n";
      msg += "Trạng thái: " + currentStatus;
      bot.sendMessage(chat_id, msg, "");
    }
  }
}

// --- HÀM BLYNK: Xử lý Gửi/Nhận dữ liệu ---
BLYNK_WRITE(V1) {
  masterLedSwitch = param.asInt(); 
  Serial.print("Blynk V1 Switch: ");
  Serial.println(masterLedSwitch);
  if (masterLedSwitch) bot.sendMessage(CHAT_ID, "Hệ thống đèn vừa được BẬT từ app Blynk 🟢", "");
  else bot.sendMessage(CHAT_ID, "Hệ thống đèn vừa bị TẮT từ app Blynk 🔴", "");
}

// Hàm chạy định kỳ 2 giây
void sendDataAndReadSensors() {
  currentT = dht.readTemperature();
  currentH = dht.readHumidity();
  currentGasValue = analogRead(MQ2PIN); 

  if (isnan(currentH) || isnan(currentT)) {
    Serial.println(F("Failed to read DHT!"));
    return;
  }

  if (currentT < 13) { currentStatus = "TOO COLD"; currentLedToBlink = LED_GREEN; } 
  else if (currentT < 20) { currentStatus = "COLD"; currentLedToBlink = LED_GREEN; }
  else if (currentT < 25) { currentStatus = "COOL"; currentLedToBlink = LED_YELLOW; }
  else if (currentT < 30) { currentStatus = "WARM"; currentLedToBlink = LED_YELLOW; }
  else if (currentT < 35) { currentStatus = "HOT"; currentLedToBlink = LED_RED; }
  else { currentStatus = "TOO HOT"; currentLedToBlink = LED_RED; }

  Blynk.virtualWrite(V0, millis() / 1000);
  Blynk.virtualWrite(V2, currentT);
  Blynk.virtualWrite(V3, currentH);
  Blynk.virtualWrite(V4, currentGasValue);
  Blynk.virtualWrite(V5, "Team 7 - Man, Dat, Oanh");

  updateOLED();
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  dht.begin();
  
  // Khởi tạo I2C cho OLED trên ESP8266
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED Error")); for(;;);
  }
  display.setTextColor(WHITE);

  // Cấu hình mạng cho Telegram
  secured_client.setInsecure(); 

  // Kết nối WiFi & Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendDataAndReadSensors);
  
  lastTempReported = dht.readTemperature();
}

void loop() {
  Blynk.run();
  timer.run();
  handleBlinking(); 

  if (millis() - bot_lasttime > 1000) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }

  if (currentT > 0 && abs(currentT - lastTempReported) >= 1.0) {
    String alertMsg = "⚠️ CẬP NHẬT NHIỆT ĐỘ!\nNhiệt độ hiện tại đã đổi thành: " + String(currentT) + "°C (" + currentStatus + ")";
    bot.sendMessage(CHAT_ID, alertMsg, "");
    lastTempReported = currentT; 
  }
}