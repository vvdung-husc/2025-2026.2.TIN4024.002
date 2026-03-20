#define BLYNK_TEMPLATE_ID "TMPL6jRSWgU4V"
#define BLYNK_TEMPLATE_NAME "ESP32 Blynk"
#define BLYNK_AUTH_TOKEN "jKSbpqUlA6joApZWXI0Yns1HOYiKq-vR"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

char ssid[] = "PBBD";
char pass[] = "123456789";

// OLED
Adafruit_SSD1306 display(128,64,&Wire,-1);

// DHT
#define DHTPIN 16
DHT dht(DHTPIN, DHT22);

// LED
#define RED 4
#define YELLOW 2
#define GREEN 15

float temp, hum;
bool systemOn = true;
int traffic = 0;

unsigned long lastDHT = 0;
unsigned long lastTraffic = 0;
unsigned long startTime;

BlynkTimer timer;

// ===== READ DHT =====
void readDHT(){
  temp = dht.readTemperature();
  hum = dht.readHumidity();
}

// ===== TRAFFIC =====
void trafficLED(){

  digitalWrite(RED,0);
  digitalWrite(YELLOW,0);
  digitalWrite(GREEN,0);

  traffic++;
  if(traffic>2) traffic=0;

  if(traffic==0) digitalWrite(GREEN,1);
  if(traffic==1) digitalWrite(YELLOW,1);
  if(traffic==2) digitalWrite(RED,1);
}

// ===== OLED =====
void showOLED(){

  display.clearDisplay();

  display.setCursor(0,0);
  display.println("ESP32 DHT Blynk");

  display.setCursor(0,16);
  display.print("Temp: "); display.println(temp);

  display.setCursor(0,30);
  display.print("Hum : "); display.println(hum);

  display.setCursor(0,46);
  display.print("Sys : ");
  display.println(systemOn?"ON":"OFF");

  display.display();
}

// ===== SEND BLYNK =====
void sendBlynk(){

  unsigned long up = (millis()-startTime)/1000;

  Blynk.virtualWrite(V0,temp);
  Blynk.virtualWrite(V1,hum);
  Blynk.virtualWrite(V4,up);
}

// ===== SWITCH =====
BLYNK_WRITE(V5){
  systemOn = param.asInt();
}

void setup(){

  Serial.begin(115200);

  pinMode(RED,OUTPUT);
  pinMode(YELLOW,OUTPUT);
  pinMode(GREEN,OUTPUT);

  Wire.begin(21,22);
  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  WiFi.begin(ssid,pass);
  while(WiFi.status()!=WL_CONNECTED) delay(200);

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  dht.begin();

  timer.setInterval(5000L,sendBlynk);

  startTime = millis();
}

void loop(){

  Blynk.run();
  timer.run();

  unsigned long now = millis();

  if(now-lastDHT>2000){
    lastDHT=now;
    readDHT();
  }

  if(systemOn && now-lastTraffic>3000){
    lastTraffic=now;
    trafficLED();
  }

  showOLED();
}