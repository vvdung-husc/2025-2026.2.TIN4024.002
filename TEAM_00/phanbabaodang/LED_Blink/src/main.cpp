#include <Arduino.h>

/* ==== PIN CONFIG ==== */
#define LED_RED     23
#define LED_YELLOW  22
#define LED_GREEN   19

/* ==== STATE MACHINE ==== */
enum TrafficState {
  RED_STATE,
  GREEN_STATE,
  YELLOW_STATE
};

struct LedConfig {
  uint8_t pin;
  uint8_t duration; // seconds
};

/* ==== CONFIG TABLE ==== */
LedConfig ledTable[] = {
  { LED_RED,    6 },
  { LED_GREEN, 10 },
  { LED_YELLOW, 4 }
};

TrafficState currentState = RED_STATE;

unsigned long lastTick = 0;
uint8_t counter = 0;
bool ledToggle = false;

/* ==== NON-BLOCKING TIMER ==== */
bool tickEvery(unsigned long &timer, uint32_t interval) {
  if (millis() - timer >= interval) {
    timer = millis();
    return true;
  }
  return false;
}

/* ==== HELPER ==== */
void turnOffAll() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
}

/* ==== SETUP ==== */
void setup() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  Serial.println("Traffic Light State Machine Started");
}

/* ==== LOOP ==== */
void loop() {

  if (tickEvery(lastTick, 1000)) {

    ledToggle = !ledToggle;
    turnOffAll();

    digitalWrite(ledTable[currentState].pin, ledToggle);
    counter++;

    Serial.print("State: ");
    Serial.print(currentState);
    Serial.print(" | Time: ");
    Serial.println(counter);

    if (counter >= ledTable[currentState].duration) {
      counter = 0;
      currentState = static_cast<TrafficState>((currentState + 1) % 3);
    }
  }
}