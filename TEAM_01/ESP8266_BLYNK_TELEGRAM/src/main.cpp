/*
THÔNG TIN NHÓM 01
1. Trần Thị Như Sang
2. Nguyễn Huy Phong
3. Đào Hữu Khánh
4. Lê Trọng Kha
5. Lê Quang Duy
*/

// ===== THƯ VIỆN =====
#include <Arduino.h>              // Thư viện cơ bản Arduino
#include <ESP8266WiFi.h>         // Kết nối WiFi cho ESP8266
#include <BlynkSimpleEsp8266.h>  // Blynk cho ESP8266
#include <WiFiClientSecure.h>    // Kết nối HTTPS (Telegram cần)
#include <UniversalTelegramBot.h>// Thư viện Telegram Bot
#include <ArduinoJson.h>         // Xử lý JSON
#include <DHT.h>                 // Cảm biến nhiệt độ & độ ẩm

// ===== WIFI =====
char ssid[] = "YOUR_WIFI";       // Tên WiFi
char pass[] = "YOUR_PASS";       // Mật khẩu WiFi

// ===== BLYNK =====
char auth[] = "YOUR_BLYNK_TOKEN"; // Token Blynk

// ===== TELEGRAM =====
#define BOT_TOKEN "YOUR_BOT_TOKEN" // Token bot Telegram
#define CHAT_ID "YOUR_CHAT_ID"     // ID chat để gửi tin

WiFiClientSecure client;          // Client bảo mật
UniversalTelegramBot bot(BOT_TOKEN, client); // Khởi tạo bot

// ===== DHT =====
#define DHTPIN D4     // Chân nối DHT
#define DHTTYPE DHT11 // Loại cảm biến
DHT dht(DHTPIN, DHTTYPE);

// ===== LED =====
#define LED D2        // Chân LED
bool ledState = false; // Trạng thái LED

// ===== TIME =====
unsigned long startTime; // Lưu thời điểm bắt đầu

// ===== BLYNK TIMER =====
BlynkTimer timer; // Timer chạy định kỳ

// ===== HÀM GỬI TELEGRAM =====
void sendTelegram(String msg) {
  bot.sendMessage(CHAT_ID, msg, ""); // Gửi tin nhắn đến chat
}

// ===== XỬ LÝ LỆNH TELEGRAM =====
void handleTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  // Duyệt từng tin nhắn
  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;

    // Bật LED
    if (text == "/led_on") {
      digitalWrite(LED, HIGH);
      ledState = true;
      sendTelegram("LED ON");
    }
    // Tắt LED
    else if (text == "/led_off") {
      digitalWrite(LED, LOW);
      ledState = false;
      sendTelegram("LED OFF");
    }
    // Kiểm tra trạng thái LED
    else if (text == "/led_status") {
      sendTelegram(ledState ? "LED đang bật" : "LED đang tắt");
    }
    // Lấy nhiệt độ & độ ẩm
    else if (text == "/get_weather") {
      float t = dht.readTemperature();
      float h = dht.readHumidity();

      String msg = "🌡 Nhiệt độ: " + String(t) +
                   "\n💧 Độ ẩm: " + String(h);
      sendTelegram(msg);
    }
  }
}

// ===== ĐIỀU KHIỂN LED TỪ BLYNK =====
BLYNK_WRITE(V0) {
  int value = param.asInt(); // Lấy giá trị từ app
  digitalWrite(LED, value);  // Bật/tắt LED
  ledState = value;          // Cập nhật trạng thái
}

// ===== GỬI DỮ LIỆU LÊN BLYNK =====
void sendData() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Nếu cảm biến lỗi → tạo dữ liệu giả
  if (isnan(t) || isnan(h)) {
    t = random(25, 35);
    h = random(60, 90);
  }

  // Gửi lên Blynk
  Blynk.virtualWrite(V1, t); // Nhiệt độ
  Blynk.virtualWrite(V2, h); // Độ ẩm

  // Lưu giá trị trước đó
  static float lastT = 0;
  static float lastH = 0;

  // Nếu thay đổi lớn → gửi Telegram
  if (abs(t - lastT) > 1 || abs(h - lastH) > 3) {
    sendTelegram("📢 Thay đổi môi trường\n🌡 " + String(t) + "\n💧 " + String(h));
    lastT = t;
    lastH = h;
  }
}

// ===== GỬI THỜI GIAN HOẠT ĐỘNG =====
void sendUptime() {
  unsigned long uptime = (millis() - startTime) / 1000; // Tính giây
  Blynk.virtualWrite(V3, uptime); // Gửi lên Blynk
}

// ===== HÀM SETUP =====
void setup() {
  Serial.begin(9600);     // Mở Serial
  pinMode(LED, OUTPUT);   // LED là output

  WiFi.begin(ssid, pass); // Kết nối WiFi
  client.setInsecure();   // Bỏ kiểm tra SSL (Telegram)

  Blynk.begin(auth, ssid, pass); // Kết nối Blynk
  dht.begin();                   // Khởi động DHT

  startTime = millis();          // Lưu thời điểm bắt đầu

  // Chạy gửi dữ liệu mỗi 5 giây
  timer.setInterval(5000L, sendData);

  // Cập nhật uptime mỗi 1 giây
  timer.setInterval(1000L, sendUptime);
}

// ===== LOOP CHÍNH =====
void loop() {
  Blynk.run();       // Chạy Blynk
  timer.run();       // Chạy timer
  handleTelegram();  // Kiểm tra lệnh Telegram
}