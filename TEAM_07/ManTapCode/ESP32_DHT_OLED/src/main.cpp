#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ======= GPIO PIN CONFIGURATION (FROM diagram.json) =======
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 16      // DHT22 data pin (from diagram: dht2:SDA -> esp:16)
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

#define LED_GREEN  15  // Green LED (from diagram: led2:A -> esp:15) 
#define LED_YELLOW 2   // Yellow LED (from diagram: r2:2 -> esp:2)
#define LED_RED    4   // Red LED (from diagram: r1:2 -> esp:4)

#define I2C_SDA 13     // OLED SDA (from diagram: oled1:SDA -> esp:13)
#define I2C_SCL 12     // OLED SCL (from diagram: oled1:SCL -> esp:12)

bool oledFound = false;

// Temperature Range Configuration
struct TemperatureRange {
  float minTemp;
  float maxTemp;
  String tempRange;
  String statusText;
  char statusIcon;  // ASCII character instead of emoji
  int ledPin;
};

TemperatureRange tempRanges[] = {
  {-100, 13, "< 13C",    "TOO COLD", '*', LED_GREEN},
  {13,   20, "13-20C",   "COLD",     '~', LED_GREEN},
  {20,   25, "20-25C",   "COOL",     '-', LED_YELLOW},
  {25,   30, "25-30C",   "WARM",     '+', LED_YELLOW},
  {30,   35, "30-35C",   "HOT",      '!', LED_RED},
  {35,   100, "> 35C",   "TOO HOT",  'X', LED_RED}
};

int numRanges = sizeof(tempRanges) / sizeof(tempRanges[0]);

// Find temperature range index
int findTemperatureRange(float temp) {
  for(int i = 0; i < numRanges; i++) {
    if(temp >= tempRanges[i].minTemp && temp < tempRanges[i].maxTemp) {
      return i;
    }
  }
  return numRanges - 1;
}

// Scan I2C devices
void scanI2C() {
  byte error, address;
  int nDevices = 0;

  Serial.println("\n=== I2C DEVICE SCAN ===");
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Found I2C device at: 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  
  if (nDevices == 0)
    Serial.println("No I2C devices found!");
  Serial.println("=== END SCAN ===\n");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n========================================");
  Serial.println("  SYSTEM STARTING - DHT22 + OLED");
  Serial.println("========================================");

  // Setup LED pins
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  // Test LEDs
  Serial.println("\n--- LED TEST ---");
  digitalWrite(LED_GREEN, HIGH);
  delay(200);
  digitalWrite(LED_GREEN, LOW);
  Serial.println("Green LED OK (GPIO 15)");
  
  digitalWrite(LED_YELLOW, HIGH);
  delay(200);
  digitalWrite(LED_YELLOW, LOW);
  Serial.println("Yellow LED OK (GPIO 2)");
  
  digitalWrite(LED_RED, HIGH);
  delay(200);
  digitalWrite(LED_RED, LOW);
  Serial.println("Red LED OK (GPIO 4)");
  
  // Initialize I2C
  Serial.println("\n--- I2C INITIALIZATION ---");
  Wire.begin(I2C_SDA, I2C_SCL);  // SDA=13, SCL=12
  delay(500);
  Wire.setClock(100000);
  delay(200);
  Serial.println("I2C initialized (SDA=13, SCL=12)");
  
  // Scan I2C devices
  scanI2C();

  // Initialize DHT22
  Serial.println("--- DHT22 INITIALIZATION ---");
  dht.begin();
  delay(100);
  Serial.println("DHT22 initialized (GPIO 16)");

  // Initialize OLED Display
  Serial.println("\n--- OLED INITIALIZATION ---");
  oledFound = false;
  byte oledAddresses[] = {0x3C, 0x3D};
  
  for(int i = 0; i < 2; i++) {
    Serial.print("Trying OLED at 0x");
    if(oledAddresses[i] < 16) Serial.print("0");
    Serial.print(oledAddresses[i], HEX);
    Serial.print("... ");
    
    if(display.begin(SSD1306_SWITCHCAPVCC, oledAddresses[i])) {
      Serial.println("OK!");
      
      // Show startup message
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 0);
      display.println("SYSTEM");
      display.setCursor(20, 16);
      display.println("READY");
      display.display();
      delay(1000);
      
      oledFound = true;
      break;
    } else {
      Serial.println("Not found");
      delay(100);
    }
  }
  
  if(!oledFound) {
    Serial.println("[ERROR] OLED display not found!");
    Serial.println("Check I2C connections: SDA=GPIO13, SCL=GPIO12");
  }
  
  Serial.println("\n========================================");
  Serial.println("  SYSTEM READY - RUNNING");
  Serial.println("========================================\n");
}

void loop() {
  delay(2500); // DHT22 needs 2+ seconds between readings

  // Read DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Check for read errors
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("[ERROR] Failed to read DHT22!");
    if(oledFound) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("DHT22 ERROR!");
      display.println("Check GPIO 16");
      display.display();
    }
    return;
  }

  // Find temperature range
  int rangeIndex = findTemperatureRange(temperature);
  String tempRange = tempRanges[rangeIndex].tempRange;
  String statusText = tempRanges[rangeIndex].statusText;
  char statusIcon = tempRanges[rangeIndex].statusIcon;
  int activeLED = tempRanges[rangeIndex].ledPin;

  // Display on OLED
  if(oledFound) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    
    // Line 1: Temperature Range
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Temp: ");
    display.println(tempRange);
    
    // Line 2: Current Temperature (large)
    display.setTextSize(2);
    display.setCursor(0, 12);
    display.print(temperature, 1);
    display.setTextSize(1);
    display.println("C");
    
    // Line 3: Humidity
    display.setTextSize(1);
    display.setCursor(0, 32);
    display.print("Hum: ");
    display.print(humidity, 0);
    display.println("%");
    
    // Line 4: Status + Icon + LED indicator
    display.setCursor(0, 45);
    display.print(statusText);
    display.print(" ");
    display.write(statusIcon);  // Print ASCII icon
    
    // LED color indicator
    display.setCursor(100, 45);
    if(activeLED == LED_GREEN) {
      display.println("G");
    } else if(activeLED == LED_YELLOW) {
      display.println("Y");
    } else if(activeLED == LED_RED) {
      display.println("R");
    }
    
    display.display();
  }

  // Print to Serial
  Serial.print("Temp: ");
  Serial.print(temperature, 1);
  Serial.print("C | Range: ");
  Serial.print(tempRange);
  Serial.print(" | Humidity: ");
  Serial.print(humidity, 0);
  Serial.print("% | Status: ");
  Serial.print(statusText);
  Serial.print(" [");
  Serial.write(statusIcon);
  Serial.println("]");

  // Control LEDs - blink effect
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  // Blink the active LED 3 times
  for(int blink = 0; blink < 3; blink++) {
    digitalWrite(activeLED, HIGH);
    delay(150);
    digitalWrite(activeLED, LOW);
    delay(150);
  }
  
  // Keep LED on for final state
  digitalWrite(activeLED, HIGH);
  delay(1200);
  digitalWrite(activeLED, LOW);
}
