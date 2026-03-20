#include <Arduino.h>

#define BLYNK_TEMPLATE_ID "TMPL65ml0WNj1"
#define BLYNK_TEMPLATE_NAME "APICANHAN"
#define BLYNK_AUTH_TOKEN "8dOqErGPbjpAPWXmQn5MAHL3Q0sCp_LH"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

struct IP4_Info{
  String ip4;
  String latitude;
  String longtitude;
};

IP4_Info ip4Info;

unsigned long currentMiliseconds = 0;
bool isApiReady = false;

bool IsReady(unsigned long &ulTimer, uint32_t milisecond) {
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

String urlWeather;

#define OPENWEATHERMAP_KEY "ceaf64165098c31663741fdb87a26310"

void parseGeoInfo(String payload, IP4_Info& ipInfo) {

  String values[7];
  int index = 0;

  while (payload.length() > 0 && index < 7) {
    int delimiterIndex = payload.indexOf('|');

    if (delimiterIndex == -1) {
      values[index++] = payload;
      break;
    }

    values[index++] = payload.substring(0, delimiterIndex);
    payload = payload.substring(delimiterIndex + 1);
  }

  ipInfo.ip4 = values[0];
  ipInfo.longtitude = values[5];
  ipInfo.latitude = values[6];

  Serial.println("===== GEO INFO =====");
  Serial.println(ipInfo.ip4);
  Serial.println(ipInfo.latitude);
  Serial.println(ipInfo.longtitude);
}

void getAPI() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi mat ket noi");
    return;
  }

  HTTPClient http;

  http.begin("http://ip4.iothings.vn/?geo=1");

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {

    String payload = http.getString();

    parseGeoInfo(payload, ip4Info);

    String mapLink = "https://www.google.com/maps?q=" + ip4Info.latitude + "," + ip4Info.longtitude;

    Serial.println(mapLink);

    urlWeather =
      "https://api.openweathermap.org/data/2.5/weather?lat=" +
      ip4Info.latitude +
      "&lon=" +
      ip4Info.longtitude +
      "&appid=" +
      OPENWEATHERMAP_KEY +
      "&units=metric";

    Serial.println(urlWeather);

    isApiReady = true;

  } else {

    Serial.print("Loi HTTP GET IP: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void updateTemp() {

  static unsigned long lastTime = 0;

  if (!IsReady(lastTime, 60000)) return;

  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;

  http.begin(urlWeather);

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {

    String response = http.getString();

    DynamicJsonDocument doc(2048);

    deserializeJson(doc, response);

    float temp = doc["main"]["temp"];

    Serial.print("Nhiet do: ");
    Serial.println(temp);

    Blynk.virtualWrite(V3, temp);

  } else {

    Serial.print("Loi HTTP Weather: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void onceCalled() {

  static bool done = false;

  if (done) return;

  done = true;

  String link = "https://www.google.com/maps?q=" + ip4Info.latitude + "," + ip4Info.longtitude;

  Blynk.virtualWrite(V1, ip4Info.ip4);
  Blynk.virtualWrite(V2, link);
}

void uptimeBlynk() {

  static unsigned long lastTime = 0;

  if (!IsReady(lastTime, 1000)) return;

  unsigned long value = currentMiliseconds / 1000;

  Blynk.virtualWrite(V0, value);
}

void setup() {

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);

  Serial.print("Dang ket noi WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  // FIX DNS
  WiFi.config(
    INADDR_NONE,
    INADDR_NONE,
    INADDR_NONE,
    IPAddress(8,8,8,8),
    IPAddress(8,8,4,4)
  );

  Serial.println(WiFi.localIP());

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  getAPI();
}

void loop() {

  Blynk.run();

  currentMiliseconds = millis();

  if (!isApiReady) {

    static unsigned long retryTime = 0;

    if (IsReady(retryTime, 5000)) {

      Serial.println("Thu lay API lai...");
      getAPI();
    }

  } else {

    onceCalled();
    updateTemp();
  }

  uptimeBlynk();
}