#include "utils.h"

// ----- LED -----
LED::LED() {
    _pin = -1;
    _state = false;
    _previousMillis = 0;
}

void LED::setup(int pin) {
    _pin = pin;
    pinMode(_pin, OUTPUT);
}

void LED::blink(unsigned long interval) {
    if (!IsReady(_previousMillis, interval)) return;
    _state = !_state;
    digitalWrite(_pin, _state ? HIGH : LOW);
}

void LED::set(bool bON) {
    digitalWrite(_pin, bON ? HIGH : LOW);
}

// ----- Trafic_Blink -----
Trafic_Blink::Trafic_Blink() {
    _ledStatus = false;
    _previousMillis = 0;
}

void Trafic_Blink::setupPin(int pinRed, int pinYellow, int pinGreen, int pinBlue, int pinButton) {
    _LEDs[0] = pinGreen;
    _LEDs[1] = pinYellow;
    _LEDs[2] = pinRed;

    pinMode(pinRed, OUTPUT);
    pinMode(pinYellow, OUTPUT);
    pinMode(pinGreen, OUTPUT);

    _idxLED = 0;
    _secondCount = 0;

    ledBlue.setup(pinBlue);
    _pinButton = pinButton;
    pinMode(_pinButton, INPUT);
}

void Trafic_Blink::setupWaitTime(uint32_t redWait, uint32_t yellowWait, uint32_t greenWait) {
    _waitTime[0] = greenWait * 1000;
    _waitTime[1] = yellowWait * 1000;
    _waitTime[2] = redWait * 1000;
    _secondCount = 0;
}

void Trafic_Blink::run(LDR& ldrSensor, TM1637Display& display, bool showLogger) {
    static uint32_t count = _waitTime[_idxLED];
    static bool prevDark = false;
    static bool prevButton = false;

    bool bButtonON = isButtonON();
    int ldrValue = ldrSensor.getValue(200);

    if (!IsReady(_previousMillis, 500)) return;

    if (prevButton != bButtonON) {
        if (!bButtonON) display.clear();
        ledBlue.set(bButtonON);
        prevButton = bButtonON;
    }

    bool isDark = (ldrValue > ldrSensor.DAY_THRESHOLD);

    if (isDark) {
        if (prevDark != isDark) {
            prevDark = isDark;
            digitalWrite(_LEDs[0], LOW);
            digitalWrite(_LEDs[2], LOW);
            printf("IT IS DARK!!!!\n");
            display.clear();
        }
        _ledStatus = !_ledStatus;
        digitalWrite(_LEDs[1], _ledStatus ? HIGH : LOW);
        return;
    }

    if (prevDark && !isDark) {
        printf("YEAH!!! IT IS DAY!!!!\n");
        prevDark = false;
        _ledStatus = false;
        _idxLED = 0;
        count = _waitTime[_idxLED];
    }

    if (count == _waitTime[_idxLED]) {
        _secondCount = (count / 1000) - 1;
        _ledStatus = true;

        for (int i = 0; i < 3; i++) {
            digitalWrite(_LEDs[i], i == _idxLED ? HIGH : LOW);
            if (i == _idxLED && showLogger)
                printf("LED [%-6s] ON => %d Seconds\n", ledString(_LEDs[i]), count / 1000);
        }
    } else {
        _ledStatus = !_ledStatus;
        digitalWrite(_LEDs[_idxLED], _ledStatus ? HIGH : LOW);
    }

    if (_ledStatus) {
        if (bButtonON) display.showNumberDec(_secondCount);
        if (showLogger)
            printf(" [%s] => seconds: %d\n", ledString(_LEDs[_idxLED]), _secondCount);
        --_secondCount;
    }

    count -= 500;
    if (count > 0) return;

    _idxLED = (_idxLED + 1) % 3;
    count = _waitTime[_idxLED];
    _secondCount = 0;
}

// ----- Button -----
bool Trafic_Blink::isButtonON() {
    static unsigned long t = 0;
    static bool state = false;
    if (!IsReady(t, 10)) return state;
    state = (digitalRead(_pinButton) == HIGH);
    return state;
}

const char* Trafic_Blink::ledString(int pin) {
    if (pin == _LEDs[2]) return "RED";
    if (pin == _LEDs[1]) return "YELLOW";
    if (pin == _LEDs[0]) return "GREEN";
    return "UNKNOWN";
}

// ----- LDR -----
int LDR::DAY_THRESHOLD = 2000;

LDR::LDR() {
    _pin = -1;
    _value = 0;
    _vcc5Volt = true;
}

void LDR::setup(int pin, bool vcc5Volt) {
    _pin = pin;
    _vcc5Volt = vcc5Volt;
    pinMode(_pin, INPUT);
}

int LDR::getValue(int timer) {
    static unsigned long t = 0;
    if (timer > 0 && !IsReady(t, timer)) return _value;
    _value = analogRead(_pin);
    return _value;
}

float LDR::readLux(int* analogValue) {
    static float prev = -1;
    float voltage, resistance, lux;

    getValue(1000);
    if (analogValue) *analogValue = _value;

    voltage = (float)_value * (_vcc5Volt ? 5.0 : 3.3) / 4095.0;
    resistance = 2000 * voltage / (1 - voltage / (_vcc5Volt ? 5.0 : 3.3));
    lux = pow((_vcc5Volt ? 50 : 33) * 1e3 * pow(10, 0.7) / resistance, (1 / 0.7));

    if (lux != prev) {
        prev = lux;
        printf("LDR: %d | Lux: %.2f\n", _value, lux);
    }
    return lux;
}

// ----- Utils -----
bool IsReady(unsigned long &ulTimer, uint32_t millisecond) {
    if (millis() - ulTimer < millisecond) return false;
    ulTimer = millis();
    return true;
}

String StringFormat(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[128];
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return String(buf);
}
