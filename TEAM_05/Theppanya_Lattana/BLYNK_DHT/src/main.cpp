#define BLYNK_TEMPLATE_ID "TMPL6IU2iv5tV"
#define BLYNK_TEMPLATE_NAME "Blynk"
#define BLYNK_AUTH_TOKEN "COyH_2Ihbnl2fyQVxSqxa1q49eabID-4"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "4 chang trai";
char pass[] = "32614004";

String API_KEY = "e761c362ad0bfcfdcf504528c72c728a";

BlynkTimer timer;

void sendData() {

  // uptime
  long uptime = millis() / 1000;
  Blynk.virtualWrite(V0, uptime);

  // IPv4
  String ip = WiFi.localIP().toString();
  Blynk.virtualWrite(V1, ip);

  // Google maps link
  String maps = "https://www.google.com/maps";
  Blynk.virtualWrite(V2, maps);

  // call weather API
  HTTPClient http;

  String url =
  "https://api.openweathermap.org/data/2.5/weather?lat=16.4666&lon=102.8333&appid="
  + API_KEY + "&units=metric";

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {

    String payload = http.getString();

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    float temp = doc["main"]["temp"];

    Blynk.virtualWrite(V3, temp);
  }

  http.end();
}

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(5000L, sendData);
}

void loop() {

  Blynk.run();
  timer.run();
}