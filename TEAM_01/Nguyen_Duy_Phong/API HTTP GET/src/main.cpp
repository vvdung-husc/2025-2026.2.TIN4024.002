#include <Arduino.h>

//Thay thông số BLYNK của bạn vào đây
#define BLYNK_TEMPLATE_ID "TMPL6TjOIJrtk"
#define BLYNK_TEMPLATE_NAME "API HTTP GET"
#define BLYNK_AUTH_TOKEN "MPKSW3OzbmMeZ5L6JCDEPqF17y1pbWmY"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <HTTPClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

// Bật MOCK DATA cho Wokwi (tắt khi upload lên ESP32 thật)
#define USE_MOCK_DATA true

struct IP4_Info{
  String ip4;
  String latitude;
  String longtitude;
};

IP4_Info ip4Info;
ulong currentMiliseconds = 0;

bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

String StringFormat(const char* fmt, ...){
  va_list vaArgs;
  va_start(vaArgs, fmt);
  va_list vaArgsCopy;
  va_copy(vaArgsCopy, vaArgs);
  const int iLen = vsnprintf(NULL, 0, fmt, vaArgsCopy);
  va_end(vaArgsCopy);
  int iSize = iLen + 1;
  char* buff = (char*)malloc(iSize);
  vsnprintf(buff, iSize, fmt, vaArgs);
  va_end(vaArgs);
  String s = buff;
  free(buff);
  return String(s);
}

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
  ipInfo.latitude = values[6].c_str();
  ipInfo.longtitude = values[5].c_str();
  
  Serial.printf("IP Address: %s\r\n", values[0].c_str());
  Serial.printf("Country Code: %s\r\n", values[1].c_str());
  Serial.printf("Country: %s\r\n", values[2].c_str());
  Serial.printf("Region: %s\r\n", values[3].c_str());
  Serial.printf("City: %s\r\n", values[4].c_str());
  Serial.printf("Longitude: %s\r\n", values[5].c_str());
  Serial.printf("Latitude: %s\r\n", values[6].c_str());
}

#define OPENWEATHERMAP_KEY "YOUR_API_KEY"
String urlWeather;

//API Get http://ip4.iothings.vn/?geo=1
void getAPI(){
  
#if USE_MOCK_DATA
  // MOCK DATA cho Wokwi
  Serial.println("========================================");
  Serial.println("⚠️  USING MOCK DATA (Wokwi Mode)");
  Serial.println("========================================");
  Serial.println("200");
  String mockResponse = "123.25.115.141|VN|Vietnam|Thừa Thiên Huế|Huế|107.5901477|16.4666697";
  Serial.println(mockResponse);
  
  parseGeoInfo(mockResponse, ip4Info);
  
  String urlGooleMaps = StringFormat("https://www.google.com/maps/place/%s,%s",ip4Info.latitude.c_str(), ip4Info.longtitude.c_str());
  Serial.printf("IPv4 => %s \r\n",ip4Info.ip4.c_str());
  Serial.println(urlGooleMaps.c_str());
  
  urlWeather = StringFormat("https://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s&units=metric",ip4Info.latitude.c_str(),ip4Info.longtitude.c_str(),OPENWEATHERMAP_KEY);
  Serial.printf("URL => %s \r\n",urlWeather.c_str());
  
#else
  // REAL API (ESP32 thật)
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("getAPI() Error in WiFi connection"); return;
  }
  HTTPClient http;   
  http.begin("http://ip4.iothings.vn/?geo=1");
  http.addHeader("Content-Type", "text/plain");

  int httpResponseCode = http.GET();
  if(httpResponseCode>0){
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
          
    parseGeoInfo(response, ip4Info);

    String urlGooleMaps = StringFormat("https://www.google.com/maps/place/%s,%s",ip4Info.latitude.c_str(), ip4Info.longtitude.c_str());
    Serial.printf("IPv4 => %s \r\n",ip4Info.ip4.c_str());
    Serial.println(urlGooleMaps.c_str());

    urlWeather = StringFormat("https://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s&units=metric",ip4Info.latitude.c_str(),ip4Info.longtitude.c_str(),OPENWEATHERMAP_KEY);

    Serial.printf("URL => %s \r\n",urlWeather.c_str());      
  }else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();
#endif
}

void updateTemp(){
  static ulong lastTime = 0;
  static float temp_ = 0.0;

  if (!IsReady(lastTime, 10000)) return;
  
#if USE_MOCK_DATA
  // MOCK Weather Data
  Serial.println("========================================");
  Serial.println("200");
  String mockWeatherResponse = "{\"coord\":{\"lon\":107.59,\"lat\":16.47},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01d\"}],\"main\":{\"temp\":26.5,\"feels_like\":26.5,\"temp_min\":26.5,\"temp_max\":26.5,\"pressure\":1013,\"humidity\":65},\"name\":\"Hue\"}";
  Serial.println(mockWeatherResponse);
  
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, mockWeatherResponse);
  if (!error) {
    float temp = doc["main"]["temp"];
    if (temp_ != temp){
      temp_ = temp;
      Serial.print("Nhiet do: "); Serial.println(temp); 
      if (Blynk.connected()) {
        Blynk.virtualWrite(V3, temp_);
      }
    }
  }
  
#else
  // REAL Weather API
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("updateTemp() Error in WiFi connection"); 
    return;
  }

  HTTPClient http;   
  http.begin(urlWeather);
  http.addHeader("Content-Type", "text/plain");
  int httpResponseCode = http.GET();
  if(httpResponseCode>0){
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
          
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, response);
    if (error) {
      Serial.println("Failed to parse JSON");
    }
    else {
      float temp = doc["main"]["temp"];
      
      if (temp_ != temp){
        temp_ = temp;
        Serial.print("Nhiet do: "); Serial.println(temp); 
        
        if (Blynk.connected()) {
          Blynk.virtualWrite(V3, temp_);
        }
      }
      
    }
  }else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();
#endif
}

void onceCalled(){
  static bool done_ = false;
  if (done_) return;
  done_ = true;
  String link = StringFormat("https://www.google.com/maps/place/%s,%s",ip4Info.latitude.c_str(),ip4Info.longtitude.c_str());

  if (Blynk.connected()) {
    Blynk.virtualWrite(V1, ip4Info.ip4.c_str());
    Blynk.virtualWrite(V2, link.c_str());
  }
}

void uptimeBlynk(){
  static ulong lastTime = 0;
  
  if (!IsReady(lastTime, 1000)) return;
  ulong value = lastTime / 1000;
  
  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, value);
  }
}

void setup(void) {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========================================");
  Serial.println("   ESP32 API - Nguyễn Duy Phong");
  Serial.println("========================================");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

#if USE_MOCK_DATA
  Serial.println("\n⚠️  MODE: WOKWI (Using Mock Data)");
  Serial.println("To use REAL API: Set USE_MOCK_DATA = false\n");
#else
  Serial.println("\n✓ MODE: ESP32 REAL (Using Real APIs)\n");
#endif

  Serial.print("Connecting to Blynk... ");
  Blynk.config(BLYNK_AUTH_TOKEN);
  bool blynkConnected = Blynk.connect(3000);
  if (blynkConnected) {
    Serial.println("Blynk OK!");
  } else {
    Serial.println("Blynk Failed (OK on Wokwi)");
  }

  getAPI();
  Serial.println("========================================\n");
}

void loop(void) {
  
  if (Blynk.connected()) {
    Blynk.run();
  }
  
  currentMiliseconds = millis();
  onceCalled(); 
  updateTemp();
  uptimeBlynk();

}