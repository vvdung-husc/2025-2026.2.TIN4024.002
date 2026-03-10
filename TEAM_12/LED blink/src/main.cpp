#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_TEMPLATE_ID "TMPL6CTPKAFXu"
#define BLYNK_TEMPLATE_NAME "ESP32 API"
#define BLYNK_AUTH_TOKEN "poiaVjEX79Vk2g3VpoivZuEg-3A03fZ7"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Chân LED
const int RED_LED = 2;
const int YELLOW_LED = 4;
const int GREEN_LED = 5;

String ip;
float lat;
float lon;
float temp;

String apiKey = "b168fb64e16342ad0c232669dd6a9f62";

BlynkTimer timer;


// Lấy IP và vị trí
void getLocation() {

  HTTPClient http;
  http.begin("http://ip4.iothings.vn/?geo=1");

  int httpCode = http.GET();

  if (httpCode > 0) {

    String payload = http.getString();

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    ip = doc["ip"].as<String>();
    lat = doc["latitude"];
    lon = doc["longitude"];

    Serial.println("IP:");
    Serial.println(ip);

    Serial.println("Latitude:");
    Serial.println(lat);

    Serial.println("Longitude:");
    Serial.println(lon);

    String mapLink =
      "https://www.google.com/maps?q=" +
      String(lat) + "," +
      String(lon);

    Blynk.virtualWrite(V1, ip);
    Blynk.virtualWrite(V2, mapLink);
  }

  http.end();
}


// Lấy nhiệt độ OpenWeather
void getWeather() {

  String url =
    "http://api.openweathermap.org/data/2.5/weather?lat=" +
    String(lat) +
    "&lon=" +
    String(lon) +
    "&appid=" +
    apiKey +
    "&units=metric";

  HTTPClient http;
  http.begin(url);

  int httpCode = http.GET();

  if (httpCode > 0) {

    String payload = http.getString();

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    temp = doc["main"]["temp"];

    Serial.print("Temperature: ");
    Serial.println(temp);

    Blynk.virtualWrite(V4, temp);
  }

  http.end();
}


// thời gian chạy
void sendUptime() {

  long uptime = millis() / 1000;

  Serial.print("Uptime: ");
  Serial.println(uptime);

  Blynk.virtualWrite(V0, uptime);
}


void setup() {

  Serial.begin(115200);
  Serial.println("ESP32 starting...");

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  WiFi.begin(ssid, pass);

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");

  Blynk.begin(auth, ssid, pass);
  Serial.println("Blynk connected!");

  // timer chạy
  timer.setInterval(10000L, getLocation);
  timer.setInterval(15000L, getWeather);
  timer.setInterval(1000L, sendUptime);
}


void loop() {

  Blynk.run();
  timer.run();
}