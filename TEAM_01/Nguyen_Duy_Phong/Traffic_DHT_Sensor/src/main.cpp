/*
 * Traffic & DHT Sensor vá»›i Blynk IoT
 * TÃ¡c giáº£: phong315
 */

#define BLYNK_TEMPLATE_ID "TMPL6H5r3Kf1c"  // â† Kiá»ƒm tra Template ID
#define BLYNK_TEMPLATE_NAME "Traffic DHT Sensor"
#define BLYNK_AUTH_TOKEN "5bq5yB0I4ZL1akxTtjEg-nBTTRMkvmka"

// Print Blynk debug
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ===== WiFi Config =====
char ssid[] = "Wokwi-GUEST";  // WiFi Wokwi
char pass[] = "";              // KhÃ´ng cÃ³ password

// ===== Virtual Pins =====
#define VPIN_TEMP      V0  // Nhiá»‡t Ä‘á»™
#define VPIN_HUMIDITY  V1  // Äá»™ áº©m
#define VPIN_UPTIME    V4  // Thá»i gian hoáº¡t Ä‘á»™ng
#define VPIN_SWITCH    V5  // Switch báº­t/táº¯t

// ===== OLED Config =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define OLED_SDA 13
#define OLED_SCL 12

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===== DHT22 Config =====
#define DHTPIN 16
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ===== LED Pins =====
#define LED_GREEN  15
#define LED_YELLOW 2
#define LED_RED    4

// ===== Biáº¿n toÃ n cá»¥c =====
float temperature = 0.0;
float humidity = 0.0;
unsigned long startTime = 0;
unsigned long uptime = 0;
bool systemEnabled = true;
bool ledState = false;

unsigned long previousDHTMillis = 0;
unsigned long previousBlinkMillis = 0;

const long dhtInterval = 2000;
const long blinkInterval = 500;

BlynkTimer timer;

// ===== Khai bÃ¡o hÃ m =====
void readDHT22();
void updateLEDs();
void updateDisplay();
void turnOffAllLEDs();
void sendToBlynk();

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Traffic & DHT Sensor ===");
  
  // LED setup
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  turnOffAllLEDs();
  
  // I2C & OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("OLED Error!"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Connecting WiFi...");
  display.display();
  
  // WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("WiFi: OK");
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display();
    delay(2000);
  } else {
    Serial.println("\nWiFi Failed!");
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("WiFi: FAILED");
    display.display();
  }
  
  // Blynk
  Serial.print("Connecting to Blynk...");
  Blynk.config(BLYNK_AUTH_TOKEN);
  if (Blynk.connect()) {
    Serial.println("Blynk Connected!");
  } else {
    Serial.println("Blynk Failed!");
  }
  
  // DHT22
  dht.begin();
  
  // Timer - gá»­i data lÃªn Blynk má»—i 5 giÃ¢y
  timer.setInterval(5000L, sendToBlynk);
  
  // Báº¯t Ä‘áº§u Ä‘áº¿m uptime
  startTime = millis();
  
  Serial.println("System Ready!");
  Serial.println("------------------------");
}

void loop() {
  Blynk.run();
  timer.run();
  
  unsigned long currentMillis = millis();
  
  // TÃ­nh uptime (giÃ¢y)
  uptime = (currentMillis - startTime) / 1000;
  
  // Äá»c DHT22 má»—i 2 giÃ¢y
  if (currentMillis - previousDHTMillis >= dhtInterval) {
    previousDHTMillis = currentMillis;
    readDHT22();
  }
  
  // Nháº¥p nhÃ¡y LED náº¿u há»‡ thá»‘ng báº­t
  if (systemEnabled) {
    if (currentMillis - previousBlinkMillis >= blinkInterval) {
      previousBlinkMillis = currentMillis;
      ledState = !ledState;
      updateLEDs();
    }
  } else {
    turnOffAllLEDs();
  }
  
  // Cáº­p nháº­t OLED
  updateDisplay();
  
  delay(10);
}

// ===== Äá»c DHT22 =====
void readDHT22() {
  float newTemp = dht.readTemperature();
  float newHum = dht.readHumidity();
  
  if (!isnan(newTemp) && !isnan(newHum)) {
    temperature = newTemp;
    humidity = newHum;
    
    Serial.print("Temp: ");
    Serial.print(temperature, 1);
    Serial.print("Â°C | Hum: ");
    Serial.print(humidity, 1);
    Serial.print("% | Uptime: ");
    Serial.print(uptime);
    Serial.println("s");
  } else {
    Serial.println("DHT22 Read Error!");
  }
}

// ===== Cáº­p nháº­t LED =====
void updateLEDs() {
  turnOffAllLEDs();
  
  if (!systemEnabled || !ledState) return;
  
  // LED xanh nháº¥p nhÃ¡y khi há»‡ thá»‘ng hoáº¡t Ä‘á»™ng
  digitalWrite(LED_GREEN, HIGH);
}

// ===== Gá»­i dá»¯ liá»‡u lÃªn Blynk =====
void sendToBlynk() {
  if (Blynk.connected()) {
    Blynk.virtualWrite(VPIN_TEMP, temperature);
    Blynk.virtualWrite(VPIN_HUMIDITY, humidity);
    Blynk.virtualWrite(VPIN_UPTIME, uptime);
    Blynk.virtualWrite(VPIN_SWITCH, systemEnabled ? 1 : 0);
    
    Serial.println("âœ“ Data sent to Blynk");
  } else {
    Serial.println("âœ— Blynk not connected");
  }
}

// ===== Cáº­p nháº­t OLED =====
void updateDisplay() {
  display.clearDisplay();
  
  // Header
  display.setTextSize(1);
  display.setCursor(5, 0);
  display.println("TRAFFIC & DHT - phong315");
  
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  
  // Uptime
  display.setCursor(0, 14);
  display.print("Uptime: ");
  display.print(uptime);
  display.println("s");
  
  // Nhiá»‡t Ä‘á»™
  display.setCursor(0, 26);
  display.println("Temp:");
  display.setTextSize(2);
  display.setCursor(0, 36);
  display.print(temperature, 1);
  display.setTextSize(1);
  display.setCursor(50, 40);
  display.println("C");
  
  // Äá»™ áº©m
  display.setCursor(70, 26);
  display.println("Hum:");
  display.setTextSize(2);
  display.setCursor(70, 36);
  display.print((int)humidity);
  display.setTextSize(1);
  display.setCursor(110, 40);
  display.println("%");
  
  display.drawLine(0, 52, 128, 52, SSD1306_WHITE);
  
  // Status
  display.setCursor(0, 56);
  display.print(systemEnabled ? "ON " : "OFF");
  
  // WiFi & Blynk status
  if (WiFi.status() == WL_CONNECTED) {
    display.setCursor(100, 56);
    display.print("W");
  }
  if (Blynk.connected()) {
    display.setCursor(110, 56);
    display.print("B");
  }
  
  display.display();
}

// ===== Táº¯t táº¥t cáº£ LED =====
void turnOffAllLEDs() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
}

// ===== BLYNK: Nháº­n lá»‡nh tá»« Switch =====
BLYNK_WRITE(V5) {
  int value = param.asInt();
  systemEnabled = (value == 1);
  
  Serial.print("Switch changed: ");
  Serial.println(systemEnabled ? "ON" : "OFF");
  
  if (!systemEnabled) {
    turnOffAllLEDs();
  }
}

// ===== BLYNK: Khi káº¿t ná»‘i thÃ nh cÃ´ng =====
BLYNK_CONNECTED() {
  Serial.println("Blynk Connected!");
  Blynk.syncVirtual(V5); // Äá»“ng bá»™ tráº¡ng thÃ¡i switch
}