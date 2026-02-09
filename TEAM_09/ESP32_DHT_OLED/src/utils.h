#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

/* ===== OLED ===== */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

extern Adafruit_SSD1306 display;

/* ===== DHT ===== */
#define DHTPIN 4
#define DHTTYPE DHT22
extern DHT dht;

/* ===== LED ===== */
#define LED_GREEN 15
#define LED_YELLOW 2
#define LED_RED 0

extern unsigned long lastBlink;
extern bool ledState;

/* ===== Functions ===== */
void setupOLED();
void updateOLED(float temp, float humi, String status);
void updateLED(float temp);
