#include <Arduino.h>

//Thay thông số BLYNK của bạn vào đây
#define BLYNK_TEMPLATE_ID "TMPL6Xxleb7qh"
#define BLYNK_TEMPLATE_NAME "BlynkAPI"
#define BLYNK_AUTH_TOKEN "MWGYyIqKlWpcUNCOxZTaiEe2YearULmj"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <HTTPClient.h>   //Thư viện gọi API
#include <ArduinoJson.h>  //Thư viện xử lý JSON

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

// Thay KEY của bạn vào đây (đã sửa lỗi dư dấu chấm phẩy)
#define OPENWEATHERMAP_KEY "fbbc54fd1cc616a99a1f1ddd4594a297"

//Cấu trúc lưu thông tin IPv4, lat, long
struct IP4_Info{
  String ip4;
  String latitude;
  String longtitude;
};

IP4_Info ip4Info; 
ulong currentMiliseconds = 0; 
String urlWeather;  

bool IsReady(ulong &ulTimer, uint32_t milisecond) {
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

//Định dạng chuỗi %s,%d,...
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

//Phân tích chuỗi trả về từ http://ip4.iothings.vn/?geo=1
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
  
  Serial.printf("IP Address: %s\r\n", values[0].c_str());
  Serial.printf("Country Code: %s\r\n", values[1].c_str());
  Serial.printf("Country: %s\r\n", values[2].c_str());
  Serial.printf("Region: %s\r\n", values[3].c_str());
  Serial.printf("City: %s\r\n", values[4].c_str());
  Serial.printf("Longitude: %s\r\n", values[5].c_str());
  Serial.printf("Latitude: %s\r\n", values[6].c_str());
}

//API Get http://ip4.iothings.vn/?geo=1
void getAPI(){
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("getAPI() Error in WiFi connection"); return;
  }
  HTTPClient http;   
  http.begin("http://ip4.iothings.vn/?geo=1");
  http.addHeader("Content-Type", "text/plain");

  int httpResponseCode = http.GET();
  if(httpResponseCode > 0){
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
          
    parseGeoInfo(response, ip4Info);

    // Đã sửa lại định dạng URL cho Google Maps
    String urlGooleMaps = StringFormat("https://www.google.com/maps?q=%s,%s", ip4Info.latitude.c_str(), ip4Info.longtitude.c_str());
    Serial.printf("IPv4 => %s \r\n", ip4Info.ip4.c_str());
    Serial.println(urlGooleMaps.c_str());

    // Đã sửa lỗi dư chữ 's' ở phần appid=%s
    urlWeather = StringFormat("https://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s&units=metric", ip4Info.latitude.c_str(), ip4Info.longtitude.c_str(), OPENWEATHERMAP_KEY);

    Serial.printf("URL Weather => %s \r\n", urlWeather.c_str());      
  }else{
    Serial.print("Error on sending GET: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// Cập nhật Nhiệt độ vào V4
void updateTemp(){
  static ulong lastTime = 0;
  static float temp_ = 0.0;

  if (!IsReady(lastTime, 10000)) return; 
  
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("updateTemp() Error in WiFi connection"); 
    return;
  }

  if (urlWeather.length() == 0) return;

  HTTPClient http;   
  http.begin(urlWeather);
  http.addHeader("Content-Type", "text/plain");
  
  int httpResponseCode = http.GET();
  if(httpResponseCode > 0){
    String response = http.getString();
          
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, response);
    if (!error) {
      float temp = doc["main"]["temp"];
      
      if (temp_ != temp){
        temp_ = temp;
        Serial.print("Nhiet do: "); Serial.println(temp); 
        Blynk.virtualWrite(V4, temp_); // Đã sửa thành V4
      }
    }
  }
  http.end();
}

// Cập nhật IP vào V2, Link vào V3, và Tên vào V0
void onceCalled(){
  static bool done_ = false;
  if (done_) return;
  
  if (ip4Info.latitude.length() > 0 && ip4Info.longtitude.length() > 0) {
    done_ = true;
    String link = StringFormat("https://www.google.com/maps?q=%s,%s", ip4Info.latitude.c_str(), ip4Info.longtitude.c_str());

    Blynk.virtualWrite(V0, "Nguyễn Trung");   // Thêm mới: Gửi tên vào V0
    Blynk.virtualWrite(V2, ip4Info.ip4.c_str());   // Đã sửa thành V2
    Blynk.virtualWrite(V3, link.c_str());          // Đã sửa thành V3
  }
}

// Cập nhật Uptime vào V1
void uptimeBlynk(){
  static ulong lastTime = 0;
  
  if (!IsReady(lastTime, 1000)) return; 
  ulong value = currentMiliseconds / 1000;
  Blynk.virtualWrite(V1, value);  // Đã sửa thành V1
}
void setup(void) {
  Serial.begin(115200);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Blynk.config(BLYNK_AUTH_TOKEN); 
  Blynk.connect();    
  
  // Thêm độ trễ 2 giây để chờ Wokwi thông mạng ra Internet
  delay(2000);

  getAPI();
}

void loop(void) {
  // Đã xóa dòng return; ở đây để vòng lặp được hoạt động
  Blynk.run();  
  
  currentMiliseconds = millis();
  onceCalled(); 
  updateTemp();
  uptimeBlynk();
}