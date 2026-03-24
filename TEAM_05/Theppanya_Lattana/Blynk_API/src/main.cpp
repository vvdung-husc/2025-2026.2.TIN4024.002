#define BLYNK_TEMPLATE_ID "TMPL6IjjqtuI9"
#define BLYNK_TEMPLATE_NAME "BlynkAPI"
#define BLYNK_AUTH_TOKEN "tlejiPCdZXwF7_OeEdySZw2qjyeJj4N2"

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>         
#include <ArduinoJson.h>       
#include <BlynkSimpleEsp32.h>


char ssid[] = "Wokwi-GUEST";
char pass[] = ""; 


String apiKey = "e761c362ad0bfcfdcf504528c72c728a";

BlynkTimer timer;

void thapThapAPI() {
  Blynk.virtualWrite(V1, millis() / 1000);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String lat = "";
    String lon = "";

    http.begin("http://ip4.iothings.vn?geo=1");
    int httpCodeGeo = http.GET();
    if (httpCodeGeo > 0) {
      String payloadGeo = http.getString();

      JsonDocument docGeo;
      deserializeJson(docGeo, payloadGeo);

      String ip = docGeo["ip"].as<String>();
      lat = docGeo["lat"].as<String>();
      lon = docGeo["lon"].as<String>();

      Blynk.virtualWrite(V2, ip);
      String linkMaps = "https://www.google.com/maps/place/" + lat + "," + lon;
      Blynk.virtualWrite(V3, linkMaps);
    }

    if (lat != "" && lon != "") {
      String urlWeather = "https://api.openweathermap.org/data/2.5/weather?lat=" + lat + "&lon=" + lon + "&appid=" + apiKey + "&units=metric";
      http.begin(urlWeather);
      int httpCodeWeather = http.GET();
      if (httpCodeWeather > 0) {
        String payloadWeather = http.getString();
        
        JsonDocument docWeather;
        deserializeJson(docWeather, payloadWeather);

        float nhietDo = docWeather["main"]["temp"];
        
        Blynk.virtualWrite(V4, nhietDo);
      }
      http.end();
    }
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(10000L, thapThapAPI);
}

void loop() {
  Blynk.run();
  timer.run();
}