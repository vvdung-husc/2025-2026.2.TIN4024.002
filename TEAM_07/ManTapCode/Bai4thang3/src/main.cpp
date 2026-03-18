#include <Arduino.h>
#define BLYNK_PRINT Serial

/* Điền thông tin Blynk của anh vào đây */
#define BLYNK_TEMPLATE_ID   "TMPL6MPV1HtAo"
#define BLYNK_TEMPLATE_NAME "ESMART"
#define BLYNK_AUTH_TOKEN    "QwvoQYeTSqLPUCQ6U3vw-OpstiKTwGup"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Thông tin WiFi
char ssid[] = "May99 Hotel";
char pass[] = "may99hotel";

// Khai báo chân Pin cho Relay
#define relayPin1 26
#define relayPin2 25
#define relayPin3 33
#define relayPin4 32

// Kết nối nút bấm trên App Blynk với Relay (inverted for active-low relays)
BLYNK_WRITE(V1) {
  digitalWrite(relayPin1, !param.asInt());
}
BLYNK_WRITE(V2) {
  digitalWrite(relayPin2, !param.asInt());
}
BLYNK_WRITE(V3) {
  digitalWrite(relayPin3, !param.asInt());
}
BLYNK_WRITE(V4) {
  digitalWrite(relayPin4, !param.asInt());
}

void setup() {
  Serial.begin(115200);
  
  // Cấu hình các chân Relay là đầu ra
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);

  // Mặc định tắt hết relay khi mới khởi động (set HIGH for active-low)
  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);
  digitalWrite(relayPin4, HIGH);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
  // Add reconnection logic
  if (!Blynk.connected()) {
    Serial.println("Reconnecting to Blynk...");
    Blynk.connect();
  }
}