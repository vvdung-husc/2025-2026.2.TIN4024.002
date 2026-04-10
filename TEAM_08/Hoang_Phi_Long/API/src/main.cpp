#define BLYNK_TEMPLATE_ID "TMPL6om1DWK2n"
#define BLYNK_TEMPLATE_NAME "API"
#define BLYNK_AUTH_TOKEN "9tzNg93bYxN-HZ9PAk9umRqbBw-Q8HXD"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

const char* owmApiKey = "a062fc47a5611d78b96ba38442327514";

#define VPIN_UPTIME  V0
#define VPIN_IPV4    V1
#define VPIN_MAPS    V2
#define VPIN_TEMP    V3

BlynkTimer timer;
unsigned long uptimeCount = 0;

float g_lat = 16.4666;
float g_lon = 107.5905;

// ================= GEO IP =================
void fetchGeoIP() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin("http://ipapi.co/json/");
  http.setTimeout(5000);

  int code = http.GET();
  Serial.println("GeoIP HTTP: " + String(code));

  if (code == HTTP_CODE_OK) {
    String raw = http.getString();

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, raw);

    if (!error) {
      String ip = doc["ip"].as<String>();
      g_lat = doc["latitude"].as<float>();
      g_lon = doc["longitude"].as<float>();

      String maps = "https://www.google.com/maps/place/" +
                    String(g_lat, 6) + "," + String(g_lon, 6);

      Blynk.virtualWrite(VPIN_IPV4, ip);
      Blynk.virtualWrite(VPIN_MAPS, maps);

      Serial.println("IP: " + ip);
      Serial.println("Maps: " + maps);
    }
  } else {
    // fallback cho Wokwi
    String maps = "https://www.google.com/maps/place/" +
                  String(g_lat, 6) + "," + String(g_lon, 6);

    Blynk.virtualWrite(VPIN_IPV4, "123.25.115.141");
    Blynk.virtualWrite(VPIN_MAPS, maps);
  }

  http.end();
}

// ================= WEATHER =================
void fetchWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Blynk.virtualWrite(VPIN_TEMP, 26.1);
    return;
  }

  HTTPClient http;

  String url = String("http://api.openweathermap.org/data/2.5/weather?lat=") +
               String(g_lat, 4) + "&lon=" + String(g_lon, 4) +
               "&appid=" + String(owmApiKey) + "&units=metric";

  http.begin(url);
  http.setTimeout(5000);

  int code = http.GET();
  Serial.println("Weather HTTP: " + String(code));

  if (code == HTTP_CODE_OK) {
    String raw = http.getString();

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, raw);

    if (!error) {
      float temp = doc["main"]["temp"].as<float>();
      Blynk.virtualWrite(VPIN_TEMP, temp);
      Serial.println("Temp: " + String(temp));
    }
  } else {
    // fallback cho Wokwi
    Blynk.virtualWrite(VPIN_TEMP, 26.1);
  }

  http.end();
}

// ================= UPTIME =================
void updateUptime() {
  uptimeCount++;
  Blynk.virtualWrite(VPIN_UPTIME, uptimeCount);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // CHỈ DÙNG BLYNK.BEGIN (đã tự connect WiFi)
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  fetchGeoIP();
  fetchWeather();

  timer.setInterval(1000L, updateUptime);
  timer.setInterval(60000L, fetchWeather);
}

// ================= LOOP =================
void loop() {
  Blynk.run();
  timer.run();
}