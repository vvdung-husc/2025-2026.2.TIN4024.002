#include "main.h"

// ===== GPIO theo diagram của BẠN =====
#define PIN_LED_RED        25
#define PIN_LED_YELLOW     33
#define PIN_LED_GREEN      32

#define PIN_LED_BLUE       21
#define PIN_BUTTON_BLUE    23

#define PIN_LDR            13   // AO LDR

#define PIN_CLK            18
#define PIN_DIO            19

// ===============================
Trafic_Blink traficLight;
LDR ldrSensor;

TM1637Display display(PIN_CLK, PIN_DIO);

void setup() {
    printf("Welcome IoT\n");

    ldrSensor.DAY_THRESHOLD = 2000;
    ldrSensor.setup(PIN_LDR, true); // LDR dùng 5V theo diagram

    traficLight.setupPin(
        PIN_LED_RED,
        PIN_LED_YELLOW,
        PIN_LED_GREEN,
        PIN_LED_BLUE,
        PIN_BUTTON_BLUE
    );

    traficLight.setupWaitTime(5, 3, 7); // RED, YELLOW, GREEN (seconds)

    display.setBrightness(0x0A);
    display.clear();
}

void loop() {
    traficLight.run(ldrSensor, display);
}
