#pragma once
#include <Arduino.h>

class LED
{
public:
    LED();
    void setup(int pin);
    void blink(unsigned long interval = 500);

private:
    int _pin;
    bool _state;
    unsigned long _previousMillis;
};

class LDR
{
public:
    LDR();
    void setup(int pin, bool vcc5Volt = true);
    int getValue();
    float readLux(int* analogValue = nullptr);

    static int DAY_THRESHOLD;

private:
    int _pin;
    int _value;
    bool _vcc5Volt;
};

class Trafic_Blink
{
public:
    Trafic_Blink();
    void setupPin(int pinRed, int pinYellow, int pinGreen);
    void setupWaitTime(uint32_t redWait = 5,
                       uint32_t yellowWait = 3,
                       uint32_t greenWait = 10);
    void run(LDR& ldrSensor);
    const char* ledString(int pin);

private:
    bool _ledStatus;
    unsigned long _previousMillis;
    int _LEDs[3];
    int _idxLED;
    uint32_t _waitTime[3];
};

bool IsReady(unsigned long& ulTimer, uint32_t millisecond = 1000);
String StringFormat(const char* fmt, ...);
