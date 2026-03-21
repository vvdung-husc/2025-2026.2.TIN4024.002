#define BLYNK_TEMPLATE_ID "TMPL6IjjqtuI9"
#define BLYNK_TEMPLATE_NAME "BlynkAPI"
#define BLYNK_AUTH_TOKEN "tlejiPCdZXwF7_OeEdySZw2qjyeJj4N2"

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>         // Thư viện để gọi API
#include <ArduinoJson.h>        // Thư viện dịch JSON
#include <BlynkSimpleEsp32.h>

char ssid[] = "Becute_5G";
char pass[] = "ttas2424";
String apiKey = "e761c362ad0bfcfdcf504528c72c728a";

BlynkTimer timer;

void thapThapAPI() {
  // 1. Gửi thời gian hoạt động (Uptime) lên V1
  Blynk.virtualWrite(V1, millis() / 1000);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String lat = "";
    String lon = "";

    // 2. Gọi API lấy IP và Toạ độ
    http.begin("http://ip4.iothings.vn?geo=1");
    int httpCodeGeo = http.GET();
    if (httpCodeGeo > 0) {
      String payloadGeo = http.getString();
      
      // Nhờ ArduinoJson dịch dữ liệu
      JsonDocument docGeo;
      deserializeJson(docGeo, payloadGeo);

      String ip = docGeo["ip"].as<String>();
      lat = docGeo["lat"].as<String>();
      lon = docGeo["lon"].as<String>();

      // Gửi IP và Link Maps lên V2, V3
      Blynk.virtualWrite(V2, ip);
      String linkMaps = "https://www.google.com/maps/place/" + lat + "," + lon;
      Blynk.virtualWrite(V3, linkMaps);
    }
    http.end();

    // 3. Gọi API lấy Nhiệt độ (Thêm &units=metric để trả về độ C)
    if (lat != "" && lon != "") {
      String urlWeather = "https://api.openweathermap.org/data/2.5/weather?lat=" + lat + "&lon=" + lon + "&appid=" + apiKey + "&units=metric";
      http.begin(urlWeather);
      int httpCodeWeather = http.GET();
      if (httpCodeWeather > 0) {
        String payloadWeather = http.getString();
        
        JsonDocument docWeather;
        deserializeJson(docWeather, payloadWeather);

        // Lấy thông số nhiệt độ (nằm trong cụm "main" -> "temp")
        float nhietDo = docWeather["main"]["temp"];
        
        // Gửi Nhiệt độ lên V4
        Blynk.virtualWrite(V4, nhietDo);
      }
      http.end();
    }
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  // Hẹn giờ cứ 10 giây đi gọi API một lần (Không để trong loop để tránh sập máy)
  timer.setInterval(10000L, thapThapAPI);
}

void loop() {
  Blynk.run();
  timer.run();
}