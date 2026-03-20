#include "utils.h"

// ================= LED =================
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

// ================= Trafic_Blink =================
Trafic_Blink::Trafic_Blink() {
    _ledStatus = false;
    _previousMillis = 0;
}

void Trafic_Blink::setupPin(int pinRed, int pinYellow, int pinGreen) {
    _LEDs[0] = pinGreen;
    _LEDs[1] = pinYellow;
    _LEDs[2] = pinRed;

    pinMode(pinRed, OUTPUT);
    pinMode(pinYellow, OUTPUT);
    pinMode(pinGreen, OUTPUT);

    _idxLED = 0;
}

void Trafic_Blink::setupWaitTime(uint32_t redWait,
                                 uint32_t yellowWait,
                                 uint32_t greenWait) {
    _waitTime[0] = greenWait * 1000;
    _waitTime[1] = yellowWait * 1000;
    _waitTime[2] = redWait * 1000;
}

void Trafic_Blink::run(LDR& ldrSensor) {
    static uint32_t count = _waitTime[_idxLED];
    static int secondCount = 0;
    static bool prevDark = false;

    if (!IsReady(_previousMillis, 500)) return;

    int adcValue;
    ldrSensor.readLux(&adcValue);
    bool isDark = (adcValue > LDR::DAY_THRESHOLD);

    if (isDark) {
        if (prevDark != isDark) {
            prevDark = isDark;
            digitalWrite(_LEDs[0], LOW);
            digitalWrite(_LEDs[2], LOW);
            printf("IT IS DARK!!!!\n");
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
        secondCount = (count / 1000) - 1;
        _ledStatus = true;

        for (int i = 0; i < 3; i++) {
            if (i == _idxLED) {
                digitalWrite(_LEDs[i], HIGH);
                printf("LED [%-6s] ON => %d Seconds\n",
                       ledString(_LEDs[i]),
                       count / 1000);
            } else {
                digitalWrite(_LEDs[i], LOW);
            }
        }
    } else {
        _ledStatus = !_ledStatus;
        digitalWrite(_LEDs[_idxLED], _ledStatus ? HIGH : LOW);
    }

    if (_ledStatus) {
        printf(" [%s] => seconds: %d \n",
               ledString(_LEDs[_idxLED]),
               secondCount);
        --secondCount;
    }

    count -= 500;
    if (count > 0) return;

    _idxLED = (_idxLED + 1) % 3;
    count = _waitTime[_idxLED];
}

const char* Trafic_Blink::ledString(int pin) {
    if (pin == _LEDs[2]) return "RED";
    if (pin == _LEDs[1]) return "YELLOW";
    if (pin == _LEDs[0]) return "GREEN";
    return "UNKNOWN";
}

// ================= LDR =================
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

int LDR::getValue() {
    _value = analogRead(_pin);
    return _value;
}

float LDR::readLux(int* analogValue) {
    static float prevLux = -1.0;
    float voltage, resistance, lux;

    getValue();
    if (analogValue) *analogValue = _value;

    voltage = (float)_value / 4096.0 * 3.3;
    resistance = 2000 * voltage / (1 - voltage / 3.3);
    lux = pow(33 * 1e3 * pow(10, 0.7) / resistance, (1 / 0.7));

    if (lux != prevLux) {
        prevLux = lux;
        printf(
            "LDR Analog: %d, Voltage: %.2f V, Resistance: %.2f Ohm, "
            "Light Intensity: %.2f[%.0f] lux\n",
            _value, voltage, resistance, lux, prevLux);
    }
    return lux;
}

// ================= Utils =================
bool IsReady(unsigned long& ulTimer, uint32_t millisecond) {
    if (millis() - ulTimer < millisecond) return false;
    ulTimer = millis();
    return true;
}

String StringFormat(const char* fmt, ...) {
    va_list vaArgs;
    va_start(vaArgs, fmt);
    char buff[256];
    vsnprintf(buff, sizeof(buff), fmt, vaArgs);
    va_end(vaArgs);
    return String(buff);
}
