/*
THÔNG TIN NHÓM 7
1. Lê Công Mẫn
2. Võ Tiến Đạt
3. Hồ Thị Kiều Oanh
*/

// --- BLYNK INFO (ĐIỀN CỦA ANH VÀO ĐÂY) ---
#define BLYNK_TEMPLATE_ID "TMPL6AwRzGKE_"
#define BLYNK_TEMPLATE_NAME "ESMart"
#define BLYNK_AUTH_TOKEN "QwvoQYeTSqLPUCQ6U3vw-OpstiKTwGup"

// --- TELEGRAM INFO (ĐIỀN CỦA ANH VÀO ĐÂY) ---
#define BOT_TOKEN "8743975552:AAHKyIiv8Nvy9kzgJNEBDCkpz-v0bkl4y7A"
#define CHAT_ID "-5177244186" // Nhớ là có dấu trừ ở đầu nhé, VD: -100123456789

#define BLYNK_PRINT Serial

#include <Arduino.h>      // Dùng cho NodeMCU (ESP8266)
#include <WiFi.h>                  // Đổi thành WiFi.h (chữ W và F viết hoa)
#include <BlynkSimpleEsp32.h>      // Đổi thành Esp32
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// --- THÔNG TIN WIFI CỦA WOKWI (KHÔNG ĐỔI) ---
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// --- 1. CẤU HÌNH CHÂN ---
#define OLED_SDA 13
#define OLED_SCL 12
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 16     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

#define LED_GREEN  15  
#define LED_YELLOW 2   
#define LED_RED    4   

// Thêm chân cho cảm biến MQ2 (Chân VP = GPIO36 theo sơ đồ anh nối)
#define MQ2PIN 36 

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
    if (chat_id != CHAT_ID) continue; // Chỉ xử lý tin nhắn trong nhóm của mình
    
    String text = bot.messages[i].text;
    Serial.print("Đã nhận lệnh Telegram: ");
    Serial.println(text);
    
    if (text == "/led_on") {
      masterLedSwitch = true;
      Blynk.virtualWrite(V1, 1); // Đồng bộ nút công tắc trên Blynk lên ON
      bot.sendMessage(chat_id, "Hệ thống đèn đã được BẬT 🟢", "");
    } 
    else if (text == "/led_off") {
      masterLedSwitch = false;
      Blynk.virtualWrite(V1, 0); // Đồng bộ nút công tắc trên Blynk về OFF
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
  // Nếu muốn điều khiển từ app thì báo luôn lên Tele
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

  // Cập nhật trạng thái đèn
  if (currentT < 13) { currentStatus = "TOO COLD"; currentLedToBlink = LED_GREEN; } 
  else if (currentT < 20) { currentStatus = "COLD"; currentLedToBlink = LED_GREEN; }
  else if (currentT < 25) { currentStatus = "COOL"; currentLedToBlink = LED_YELLOW; }
  else if (currentT < 30) { currentStatus = "WARM"; currentLedToBlink = LED_YELLOW; }
  else if (currentT < 35) { currentStatus = "HOT"; currentLedToBlink = LED_RED; }
  else { currentStatus = "TOO HOT"; currentLedToBlink = LED_RED; }

  // Gửi lên Blynk
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
  
  // Khởi tạo nhiệt độ ban đầu
  lastTempReported = dht.readTemperature();
}

void loop() {
  Blynk.run();
  timer.run();
  handleBlinking(); 

  // 1. Check tin nhắn Telegram (cứ 1 giây quét 1 lần)
  if (millis() - bot_lasttime > 1000) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }

  // 2. Tự động báo Telegram nếu nhiệt độ thay đổi >= 1 độ C
  if (currentT > 0 && abs(currentT - lastTempReported) >= 1.0) {
    String alertMsg = "⚠️ CẬP NHẬT NHIỆT ĐỘ!\nNhiệt độ hiện tại đã đổi thành: " + String(currentT) + "°C (" + currentStatus + ")";
    bot.sendMessage(CHAT_ID, alertMsg, "");
    lastTempReported = currentT; // Cập nhật lại mốc để so sánh lần sau
  }
}