#define BLYNK_PRINT Serial // Dòng này giúp in log của Blynk ra Terminal
#define BLYNK_TEMPLATE_ID "TMPL61jQDGGP6"
#define BLYNK_TEMPLATE_NAME "Traffic DHT Sensor"
#define BLYNK_AUTH_TOKEN "VYvTHToUHJOeWdTuxn3Y26NdM9oGUPCn"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TM1637Display.h>

#define DHTPIN 15      
#define DHTTYPE DHT22
#define CLK 22         
#define DIO 23         
#define LED_PIN 2      

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);
BlynkTimer timer;

int activeTime = 0; 
bool isRunning = false;

// Hàm gửi Nhiệt độ & Độ ẩm lên Blynk
void sendSensorData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Lỗi: Không đọc được cảm biến DHT22!");
    return;
  }
  
  // In ra Terminal để bạn dễ quan sát
  Serial.print("Nhiệt độ: "); Serial.print(t); Serial.print("°C \t");
  Serial.print("Độ ẩm: "); Serial.print(h); Serial.println("%");

  Blynk.virtualWrite(V1, t); 
  Blynk.virtualWrite(V2, h); 
}

// Hàm chạy thời gian hoạt động
void updateUptime() {
  if (isRunning) {
    digitalWrite(LED_PIN, HIGH);       
    display.showNumberDec(activeTime); 
    Blynk.virtualWrite(V3, activeTime);
    
    Serial.print("Thời gian chạy: "); Serial.println(activeTime);
    activeTime++; 
  } else {
    digitalWrite(LED_PIN, LOW);        
    display.clear();                   
    activeTime = 0;                    
  }
}

// Hàm này sẽ tự động chạy khi bạn bấm nút trên app Blynk
BLYNK_WRITE(V0) {
  isRunning = param.asInt();
  Serial.print("ĐÃ NHẬN LỆNH TỪ BLYNK! Trạng thái công tắc: ");
  Serial.println(isRunning ? "BẬT" : "TẮT");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  display.setBrightness(0x0f);
  dht.begin();
  
  Serial.println("Đang kết nối WiFi và Blynk...");
  Blynk.begin(auth, ssid, pass);

  Blynk.virtualWrite(V4, "Võ Tiến Đạt");
  
  timer.setInterval(2000L, sendSensorData); 
  timer.setInterval(1000L, updateUptime);   
}

void loop() {
  Blynk.run();
  timer.run();
}