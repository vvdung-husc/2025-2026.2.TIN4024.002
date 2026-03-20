#include <Arduino.h>

/* ===== PIN CONFIG (KHỚP diagram.json) ===== */
#define LED_RED     23
#define LED_GREEN   19
#define LED_YELLOW  22

/* ===== STATE MACHINE ===== */
enum TrafficState {
  RED_STATE,
  GREEN_STATE,
  YELLOW_STATE
};

/* ===== LED CONFIG TABLE ===== */
struct LedConfig {
  uint8_t pin;
  uint8_t duration;   // seconds
};

LedConfig ledTable[] = {
  { LED_RED,    6 },   // Đèn đỏ 6s
  { LED_GREEN, 10 },   // Đèn xanh 10s
  { LED_YELLOW, 4 }    // Đèn vàng 4s
};

TrafficState currentState = RED_STATE;

unsigned long lastTick = 0;
uint8_t counter = 0;

/* ===== TIMER (NON-BLOCKING) ===== */
bool tickEvery(unsigned long &timer, uint32_t interval) {
  if (millis() - timer >= interval) {
    timer = millis();
    return true;
  }
  return false;
}

/* ===== HELPER ===== */
void turnOffAll() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
}

const char* stateName(TrafficState state) {
  switch (state) {
    case RED_STATE:    return "RED";
    case GREEN_STATE:  return "GREEN";
    case YELLOW_STATE: return "YELLOW";
    default:           return "UNKNOWN";
  }
}

/* ===== SETUP ===== */
void setup() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  turnOffAll();
  digitalWrite(LED_RED, HIGH);   // bắt đầu với đèn đỏ

  Serial.println("🚦 Traffic Light Started");
}

/* ===== LOOP ===== */
void loop() {

  if (tickEvery(lastTick, 1000)) {

    counter++;

    Serial.print("State: ");
    Serial.print(stateName(currentState));
    Serial.print(" | Time: ");
    Serial.println(counter);

    if (counter >= ledTable[currentState].duration) {

      counter = 0;
      currentState = static_cast<TrafficState>((currentState + 1) % 3);

      turnOffAll();
      digitalWrite(ledTable[currentState].pin, HIGH);
    }
  }
}
