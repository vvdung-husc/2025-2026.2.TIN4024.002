#define BLYNK_TEMPLATE_ID "TMPL6cjur1LZR"
#define BLYNK_TEMPLATE_NAME "Temperature"
#define BLYNK_AUTH_TOKEN "_8-tl8qgzS0uPmU3b0D4dvqLG6ZfqbiJ"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TM1637Display.h>

// WiFi
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// ===== LED =====
#define LED_PIN 21

// ===== DHT =====
#define DHTPIN 16
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ===== TM1637 =====
#define CLK 18
#define DIO 19
TM1637Display display(CLK, DIO);

// ===== Timer =====
BlynkTimer timer;
int timeRun = 0;

// ===== Điều khiển LED từ app =====
BLYNK_WRITE(V2) {
  int value = param.asInt();
  digitalWrite(LED_PIN, value);

  Serial.print("LED: ");
  Serial.println(value);
}

// ===== Gửi dữ liệu =====
void sendData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(temp) && !isnan(hum)) {
    Serial.print("Temp: ");
    Serial.println(temp);
    Serial.print("Hum: ");
    Serial.println(hum);

    // Gửi lên Blynk
    Blynk.virtualWrite(V0, temp);
    Blynk.virtualWrite(V1, hum);

    // Hiển thị nhiệt độ lên LED 4 số
    display.showNumberDec((int)temp);
  }

  // Thời gian chạy
  timeRun++;
  Blynk.virtualWrite(V3, timeRun);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin();

  display.setBrightness(7);

  // Kết nối WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi OK");

  // Kết nối Blynk
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  if (Blynk.connected()) {
    Serial.println("Blynk OK");
  }

  // Timer gửi dữ liệu mỗi 2s
  timer.setInterval(2000L, sendData);
}

void loop() {
  Blynk.run();
  timer.run();
}