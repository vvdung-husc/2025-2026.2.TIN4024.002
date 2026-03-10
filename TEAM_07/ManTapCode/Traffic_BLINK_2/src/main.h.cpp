#include "main.h"

// ========== LED Class ==========
LED::LED() : _pin(-1), _status(false), _name("")
{
}

LED::~LED()
{
}

const char *LED::getName()
{
    return _name.c_str();
}

void LED::setup(int pin, const char *name)
{
    _pin = pin;
    _name = name;
    pinMode(_pin, OUTPUT);
    setStatus(false);
}

bool LED::getStatus()
{
    return _status;
}

void LED::setStatus(bool bON)
{
    _status = bON;
    digitalWrite(_pin, bON ? HIGH : LOW);
}

void LED::blink()
{
    setStatus(!getStatus());
}

// ========== Traffic_Blink Class ==========
Traffic_Blink::Traffic_Blink() : _idxLED(0)
{
    memset(_waitTime, 0, sizeof(_waitTime));
}

Traffic_Blink::~Traffic_Blink()
{
}

void Traffic_Blink::setup_Pin(int pinRed, int pinYellow, int pinGreen)
{
    _leds[0].setup(pinRed, "RED");
    _leds[1].setup(pinYellow, "YELLOW");
    _leds[2].setup(pinGreen, "GREEN");
    _idxLED = 0;
    _leds[_idxLED].setStatus(true);
}

void Traffic_Blink::setup_WaitTime(int redTimer, int yellowTimer, int greenTimer)
{
    _waitTime[0] = redTimer * 1000;    // RED
    _waitTime[1] = yellowTimer * 1000; // YELLOW
    _waitTime[2] = greenTimer * 1000;  // GREEN
}

void Traffic_Blink::blink()
{
    static unsigned long lastSwitch = 0;
    
    if (IsReady(lastSwitch, _waitTime[_idxLED]))
    {
        _leds[_idxLED].setStatus(false);
        _idxLED = (_idxLED + 1) % 3;
        _leds[_idxLED].setStatus(true);
    }
}

// ========== Helper Functions ==========
bool IsReady(unsigned long &ulTimer, uint32_t millisecond)
{
    unsigned long now = millis();
    if (now - ulTimer >= millisecond)
    {
        ulTimer = now;
        return true;
    }
    return false;
}

String StringFormat(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    
    va_end(args);
    return String(buffer);
}
