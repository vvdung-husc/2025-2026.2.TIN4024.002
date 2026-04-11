#include "main.h"

// ===== Định nghĩa chân LED theo diagram =====
#define PIN_LED_RED     25
#define PIN_LED_YELLOW  33
#define PIN_LED_GREEN   32

// ===== LDR =====
#define PIN_LDR 34   // GPIO34 - ADC

int DAY_ADC_THRESHOLD = 2000;

// ===== Object =====
Trafic_Blink traficLight;
LDR ldrSensor;

void setup() {
    printf("Welcome IoT\n");

    // LDR dùng nguồn 3.3V
    ldrSensor.DAY_THRESHOLD = DAY_ADC_THRESHOLD;
    ldrSensor.setup(PIN_LDR, false);

    // LED giao thông
    traficLight.setupPin(
        PIN_LED_RED,
        PIN_LED_YELLOW,
        PIN_LED_GREEN
    );

    traficLight.setupWaitTime(5, 3, 7); // RED, YELLOW, GREEN (seconds)
}

void loop() {
    traficLight.run(ldrSensor);
}
