#define BLYNK_TEMPLATE_ID "TMPL67JgaGRnk"
#define BLYNK_TEMPLATE_NAME "API"
#define BLYNK_AUTH_TOKEN "ANrvOPoy83VZtJSU8-9BA39w3l1nLWmc"


#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp32.h>

const char* ssid      = "Wokwi-GUEST";
const char* password  = "";
const char* owmApiKey = "ANrvOPoy83VZtJSU8-9BA39w3l1nLWmc";

#define VPIN_UPTIME  V0
#define VPIN_IPV4    V1
#define VPIN_MAPS    V2
#define VPIN_TEMP    V3

BlynkTimer timer;
unsigned long uptimeCount = 0;
float g_lat = 16.4666;
float g_lon = 107.5905;

void fetchGeoIP() {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  http.begin("http://ipapi.co/json/");
  http.addHeader("User-Agent", "ESP32HTTPClient");
  http.setTimeout(10000);
  int code = http.GET();
  Serial.println("GeoIP HTTP: " + String(code));
  if (code == HTTP_CODE_OK) {
    String raw = http.getString();
    Serial.println(raw);
    StaticJsonDocument<1024> doc;
    if (!deserializeJson(doc, raw)) {
      String ip = doc["ip"].as<String>();
      g_lat = doc["latitude"].as<float>();
      g_lon = doc["longitude"].as<float>();
      String maps = "https://www.google.com/maps/place/"
                    + String(g_lat, 7) + "," + String(g_lon, 7);
      Blynk.virtualWrite(VPIN_IPV4, ip);
      Blynk.virtualWrite(VPIN_MAPS, maps);
      Serial.println("IP: " + ip);
      Serial.println("Maps: " + maps);
    }
  } else {
    String maps = "https://www.google.com/maps/place/"
                  + String(g_lat, 7) + "," + String(g_lon, 7);
    Blynk.virtualWrite(VPIN_IPV4, "123.25.115.141");
    Blynk.virtualWrite(VPIN_MAPS, maps);
  }
  http.end();
}

void fetchWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    // Wokwi chặn HTTP -> dung gia tri cung de Blynk hien thi
    Blynk.virtualWrite(VPIN_TEMP, 26.1);
    Serial.println("Weather: using fallback temp 26.1");
    return;
  }

  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?lat="
               + String(g_lat, 4) + "&lon=" + String(g_lon, 4)
               + "&appid=" + String(owmApiKey) + "&units=metric";

  http.begin(url);
  http.setTimeout(8000);
  int code = http.GET();
  Serial.println("Weather HTTP: " + String(code));

  if (code == HTTP_CODE_OK) {
    String raw = http.getString();
    StaticJsonDocument<1024> doc;
    if (!deserializeJson(doc, raw)) {
      float temp = doc["main"]["temp"].as<float>();
      Serial.println("Temp: " + String(temp));
      Blynk.virtualWrite(VPIN_TEMP, temp);
    }
  } else {
    // API bi chan -> dung gia tri cung
    Blynk.virtualWrite(VPIN_TEMP, 26.1);
    Serial.println("Weather blocked, fallback 26.1");
  }
  http.end();
}

void updateUptime() {
  uptimeCount++;
  Blynk.virtualWrite(VPIN_UPTIME, uptimeCount);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry++ < 30) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? " WiFi OK!" : " WiFi FAIL!");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  fetchGeoIP();
  delay(2000);
  fetchWeather();
  timer.setInterval(1000L,  updateUptime);
  timer.setInterval(60000L, fetchWeather);
}

void loop() {
  Blynk.run();
  timer.run();
}