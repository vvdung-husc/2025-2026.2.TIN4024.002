#define BLYNK_TEMPLATE_ID "TMPL6NezR9rkf"
#define BLYNK_TEMPLATE_NAME "DHT"
#define BLYNK_AUTH_TOKEN "Bbsq-Y9Qjb9PFLX8Vjcttwk1ySkj9IOW"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TM1637Display.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// DHT22
#define DHTPIN 16
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// TM1637
#define CLK 18
#define DIO 19
TM1637Display display(CLK, DIO);

// LED + BUTTON
#define LED_PIN 21
#define BUTTON_PIN 23

// Virtual Pins
#define VPIN_TEMP V0
#define VPIN_HUM  V1
#define VPIN_LED  V2
#define VPIN_TIME V3

BlynkTimer timer;

int seconds = 0;
bool ledState = false;

void sendSensor()
{
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum))
  {
    Serial.println("DHT error");
    return;
  }

  Blynk.virtualWrite(VPIN_TEMP, temp);
  Blynk.virtualWrite(VPIN_HUM, hum);

  Serial.print("Temp: ");
  Serial.println(temp);

  Serial.print("Hum: ");
  Serial.println(hum);
}

void sendTime()
{
  seconds++;
  Blynk.virtualWrite(VPIN_TIME, seconds);

  // Hiển thị lên TM1637
  display.showNumberDec(seconds, false);
}

// Điều khiển LED từ Blynk
BLYNK_WRITE(VPIN_LED)
{
  ledState = param.asInt();

  digitalWrite(LED_PIN, ledState);

  if (ledState)
    Serial.println("Blue Light ON");
  else
    Serial.println("Blue Light OFF");
}

void checkButton()
{
  if (digitalRead(BUTTON_PIN) == LOW)
  {
    ledState = !ledState;

    digitalWrite(LED_PIN, ledState);
    Blynk.virtualWrite(VPIN_LED, ledState);

    delay(300);
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  display.setBrightness(7);

  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendSensor);
  timer.setInterval(1000L, sendTime);
}

void loop()
{
  Blynk.run();
  timer.run();
  checkButton();
}