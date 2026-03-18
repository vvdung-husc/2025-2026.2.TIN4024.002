/*
 * THÔNG TIN NHÓM XY.002
 * 1. Nguyễn Văn Bình
 * 2. Lê Nguyễn Hương Nguyên
 */

// --- BẮT BUỘC ĐIỀN SAU KHI CÓ TÀI KHOẢN BLYNK ---
#define BLYNK_TEMPLATE_ID "TMPL6xQMOn5tP"
#define BLYNK_TEMPLATE_NAME "ESP8266 BLYNK TELEGRAM"
#define BLYNK_AUTH_TOKEN "DIEN_TOKEN_CUA_BAN"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <TM1637Display.h> // Dùng thư viện này thay cho OLED nếu bạn dùng màn hình 7 đoạn như trong hình

// --- CẤU HÌNH ---
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST"; 
char pass[] = "";

#define DHTPIN 15 // Chân D8 trên NodeMCU
#define LED_PIN 2 // Chân LED tích hợp
#define CLK 14    // Chân D5 cho màn hình 7 đoạn
#define DIO 12    // Chân D6 cho màn hình 7 đoạn

DHT dht(DHTPIN, DHT22);
TM1637Display display(CLK, DIO);
BlynkTimer timer;

void sendSensorData() {
    float t = dht.readTemperature();
    if (isnan(t)) return;

    Blynk.virtualWrite(V1, t); // Gửi nhiệt độ lên Blynk
    display.showNumberDec(t);  // Hiển thị lên màn hình 4 số
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    dht.begin();
    display.setBrightness(0x0f);
    
    Blynk.begin(auth, ssid, pass);
    timer.setInterval(2000L, sendSensorData);
}

void loop() {
    Blynk.run();
    timer.run();
}