#define BLYNK_TEMPLATE_ID "TMPL64jegcCVd"
#define BLYNK_TEMPLATE_NAME "VANLINH"
#define BLYNK_AUTH_TOKEN "0brrj_KPFhentpz0jUourKsRsRyAZYig"

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <TM1637Display.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

/* WIFI (SỬA LẠI WIFI NHÀ BẠN) */
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

/* PIN CONFIG */
#define LED_PIN 21
#define BUTTON_PIN 23

#define CLK 18
#define DIO 19

#define DHT_PIN 16
#define DHTTYPE DHT22

/* OBJECT */
TM1637Display display(CLK, DIO);
BlynkTimer timer;
DHT dht(DHT_PIN, DHTTYPE);

/* VARIABLE */
bool ledState = false;
bool lastButtonState = HIGH;

unsigned long startTime = 0;
unsigned long totalTime = 0;

/* ===== HÀM BẬT TẮT LED ===== */
void setLed(bool state)
{
  if (state == ledState) return;

  ledState = state;
  digitalWrite(LED_PIN, ledState);

  if (ledState)
  {
    startTime = millis();
  }
  else
  {
    totalTime += millis() - startTime;
  }
}

/* ===== BLYNK SWITCH ===== */
BLYNK_WRITE(V0)
{
  int value = param.asInt();
  setLed(value);
}

/* ===== BUTTON CONTROL ===== */
void checkButton()
{
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH)
  {
    setLed(!ledState);
    Blynk.virtualWrite(V0, ledState);
    delay(200);
  }

  lastButtonState = buttonState;
}

/* ===== UPDATE DISPLAY ===== */
void updateDisplay()
{
  unsigned long currentTime = totalTime;

  if (ledState)
  {
    currentTime += millis() - startTime;
  }

  int seconds = currentTime / 1000;

  display.showNumberDec(seconds, true);
  Blynk.virtualWrite(V3, seconds);
}

/* ===== READ DHT22 ===== */
void readDHT()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("DHT read failed");
    return;
  }

  Serial.print("Temp: ");
  Serial.println(temperature);

  Serial.print("Hum: ");
  Serial.println(humidity);

  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
}

/* ===== SETUP ===== */
void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  display.setBrightness(7);
  dht.begin();

  /* KẾT NỐI WIFI */
  WiFi.begin(ssid, pass);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");

  /* KẾT NỐI BLYNK */
  Blynk.config(BLYNK_AUTH_TOKEN);

  if (Blynk.connect())
  {
    Serial.println("Blynk connected");
  }
  else
  {
    Serial.println("Blynk connection failed");
  }

  /* TIMER */
  timer.setInterval(100L, checkButton);
  timer.setInterval(1000L, updateDisplay);
  timer.setInterval(2000L, readDHT);
}

/* ===== LOOP ===== */
void loop()
{
  Blynk.run();
  timer.run();
}