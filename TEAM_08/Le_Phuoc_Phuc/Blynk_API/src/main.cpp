#define BLYNK_TEMPLATE_ID "TMPL65c0eOt5R"
#define BLYNK_TEMPLATE_NAME "API"
#define BLYNK_AUTH_TOKEN "06N8RnCmoiV0mpsFslY3dtZrrhzeuR8k"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TM1637Display.h>
#include <DHT.h>

// ===== DHT =====
#define DHTTYPE DHT22
#define DHT_PIN 16

// ===== LED DISPLAY =====
#define CLK_PIN 18
#define DIO_PIN 19

// ===== WIFI =====
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

// ===== API =====
const char* OWM_API_KEY = "152fabd9fda5f1766d54e461cd9e590f";
const char* GEO_API_URL = "http://ip4.iothings.vn/?geo=1";

// ===== INTERVAL =====
#define UPTIME_INTERVAL  1000L
#define WEATHER_INTERVAL 30000L
#define DHT_INTERVAL     5000L

// ===== OBJECT =====
TM1637Display display(CLK_PIN, DIO_PIN);
DHT dht(DHT_PIN, DHTTYPE);
BlynkTimer timer;

// ===== STATE =====
struct GeoInfo {
  String ip;
  String lat;
  String lon;
  bool valid = false;
} geo;

unsigned long uptimeSec = 0;

// ============================================================
// WIFI
// ============================================================
void connectWiFi() {
  Serial.println("[WiFi] Connecting...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[WiFi] Connected!");
  Serial.println(WiFi.localIP());
}

// ============================================================
// HTTP GET
// ============================================================
String httpGET(String url) {
  if (WiFi.status() != WL_CONNECTED) return "";

  HTTPClient http;
  http.begin(url);
  int code = http.GET();

  String payload = "";
  if (code == 200) {
    payload = http.getString();
  }

  http.end();
  return payload;
}

// ============================================================
// GEO LOCATION
// ============================================================
void getIPLocation() {
  String raw = httpGET(GEO_API_URL);
  if (raw == "") return;

  String data[7];
  int index = 0;

  while (raw.length() && index < 7) {
    int pos = raw.indexOf('|');
    if (pos == -1) {
      data[index++] = raw;
      break;
    }
    data[index++] = raw.substring(0, pos);
    raw.remove(0, pos + 1);
  }

  geo.ip  = data[0];
  geo.lon = data[5];
  geo.lat = data[6];
  geo.valid = true;

  Serial.println("[GEO] IP: " + geo.ip);

  // 👉 Blynk
  Blynk.virtualWrite(V1, geo.ip);
  Blynk.virtualWrite(V4, geo.lat.toFloat(), geo.lon.toFloat()); // MAP
}

// ============================================================
// WEATHER API
// ============================================================
void getWeather() {
  if (!geo.valid) return;

  String url = "https://api.openweathermap.org/data/2.5/weather"
               "?lat=" + geo.lat +
               "&lon=" + geo.lon +
               "&appid=" + OWM_API_KEY +
               "&units=metric";

  String body = httpGET(url);
  if (body == "") return;

  JsonDocument doc;
  deserializeJson(doc, body);

  float temp = doc["main"]["temp"];

  Serial.print("[API TEMP] ");
  Serial.println(temp);

  Blynk.virtualWrite(V3, temp);
}

// ============================================================
// DHT SENSOR
// ============================================================
void readDHT() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("[DHT] Error");
    return;
  }

  Serial.print("[DHT] Temp: ");
  Serial.print(temp);
  Serial.print("  Hum: ");
  Serial.println(hum);

  Blynk.virtualWrite(V5, temp);
  Blynk.virtualWrite(V6, hum);
}

// ============================================================
// UPTIME
// ============================================================
void updateUptime() {
  uptimeSec = millis() / 1000;

  display.showNumberDec(uptimeSec % 10000);
  Blynk.virtualWrite(V0, uptimeSec);
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);

  display.setBrightness(7);
  display.showNumberDec(0);

  dht.begin();

  connectWiFi();

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  getIPLocation();
  getWeather();

  timer.setInterval(UPTIME_INTERVAL, updateUptime);
  timer.setInterval(WEATHER_INTERVAL, getWeather);
  timer.setInterval(DHT_INTERVAL, readDHT);
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  Blynk.run();
  timer.run();
}