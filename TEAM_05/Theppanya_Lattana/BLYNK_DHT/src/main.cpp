
#define BLYNK_TEMPLATE_ID "TMPL6IUZiv5tV"
#define BLYNK_TEMPLATE_NAME "Blynk"
#define BLYNK_AUTH_TOKEN "COyH_2Ihbnl2fyQVxSqxa1q49eabID-4"

#include <Arduino.h> 
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TM1637Display.h>

char ssid[] = "Ko co mang";
char pass[] = "Brmilahud";


#define DHTPIN 16      
#define DHTTYPE DHT22     

#define CLK_PIN 18         
#define DIO_PIN 19        

#define LED_PIN 21        
#define BUTTON_PIN 23      


DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK_PIN, DIO_PIN);
BlynkTimer timer;

int uptimeCount = 0;

void sendSensorData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Lỗi không đọc được DHT!");
    return;
  }

  Blynk.virtualWrite(V0, t); 
  Blynk.virtualWrite(V1, h); 

  uptimeCount++;
  Blynk.virtualWrite(V2, uptimeCount); 

  display.showNumberDec(uptimeCount, false);
}

BLYNK_WRITE(V3) {
  int switchState = param.asInt(); 
  
  if (switchState == 1) {
    digitalWrite(LED_PIN, HIGH); 
    Serial.println("Bật đèn & Chế độ đếm ngược");
  } else {
    digitalWrite(LED_PIN, LOW);  
    Serial.println("Tắt đèn");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  display.setBrightness(0x0f); 
  display.clear();

  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}