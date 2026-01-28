# Bài tập IoT - Lê Trọng Kha

**Mã sinh viên:** 22t1020634  
**Lớp:** TIN4024.002  
**Năm học:** 2025-2026 Học kỳ 2

## Dự án 1: LED Blink

### Mô tả
Dự án cơ bản về điều khiển đèn LED nhấp nháy sử dụng vi mạch ESP32.

### Tính năng
- Đèn LED đỏ nhấp nháy với chu kỳ 1 giây (bật 1s, tắt 1s)
- Sử dụng phương pháp non-blocking để không làm treo chương trình
- Hiển thị trạng thái LED qua Serial Monitor

### Linh kiện sử dụng
- ESP32 DevKit C v4
- LED đỏ
- Điện trở 220Ω
- Dây nối

### Sơ đồ kết nối
- LED Anode → ESP32 Pin 23
- LED Cathode → Điện trở 220Ω → ESP32 GND

### Cách chạy
1. Mở project trong VS Code với PlatformIO
2. Kết nối ESP32 hoặc chạy simulation với Wokwi
3. Build và upload code
4. Mở Serial Monitor để xem trạng thái LED

### Code chính
```cpp
#include <Arduino.h>

#define PIN_LED_RED 23

bool IsReady(unsigned long &ulTimer, uint32_t millisecond) {
  if (millis() - ulTimer < millisecond) return false;
  ulTimer = millis();
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("LED Blink Project - Le Trong Kha");
  pinMode(PIN_LED_RED, OUTPUT);
}

void loop() {
  static unsigned long ulTimer = 0;
  static bool ledStatus = false;

  if (IsReady(ulTimer, 1000)) {
    ledStatus = !ledStatus;
    Serial.printf("LED is %s\n", ledStatus ? "ON" : "OFF");
    digitalWrite(PIN_LED_RED, ledStatus ? HIGH : LOW);
  }
}
```