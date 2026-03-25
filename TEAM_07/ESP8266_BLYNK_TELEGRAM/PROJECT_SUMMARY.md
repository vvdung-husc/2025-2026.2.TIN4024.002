# 📋 Project Summary & File Structure

## 📁 Cấu trúc Dự án

```
ESP8266_BLYNK_TELEGRAM/                    # Thư mục chính
│
├── 📄 README.md                           # Tài liệu chính (Đọc ngay!)
├── 📄 SETUP.md                            # Hướng dẫn cài đặt nhanh (5 bước)
├── 📄 BLYNK_SETUP.md                      # Hướng dẫn Blynk detail
├── 📄 TELEGRAM_SETUP.md                   # Hướng dẫn Telegram Bot detail
├── 📄 PROJECT_SUMMARY.md                  # File này
│
├── 📄 platformio.ini                      # Cấu hình PlatformIO
├── 📄 wokwi.toml                          # Cấu hình Wokwi Simulator
├── 📄 diagram.json                        # Sơ đồ mạch
│
├── 📁 src/                                # Source code
│   └── 📄 main.cpp                        # ← CODE CHÍNH (HOÀN CHỈNH)
│
├── 📁 include/                            # Header files
│   ├── 📄 README
│   └── 📄 config.h                        # Configuration template
│
├── 📁 lib/                                # Libraries
│   └── 📄 README
│
└── 📁 test/                               # Test files
    └── 📄 README
```

---

## 🎯 Mục tiêu dự án

**Xây dựng hệ thống IoT giám sát nhiệt độ và độ ẩm với các tính năng:**

| Tính năng | Status | Chi tiết |
|---|:---:|---|
| Đọc cảm biến DHT22 | ✅ | Mỗi 2 giây |
| Hiển thị OLED | ✅ | Real-time, nhiệt độ/độ ẩm/trạng thái |
| Điều khiển LED | ✅ | 3 LED, tự động theo nhiệt độ |
| Kết nối WiFi | ✅ | DHCP/Static IP |
| Blynk IoT | ✅ | Gửi dữ liệu real-time & điều khiển từ xa |
| Telegram Bot | ✅ | Commands & cảnh báo tự động |
| Serial Debug | ✅ | 115200 baud |

---

## 📚 Hệ thống các file tài liệu

### 1. **README.md** - Tài liệu chính
```
Nội dung bao gồm:
- Thông tin nhóm & mục tiêu dự án
- Thiết bị & sơ đồ kết nối
- Hướng dẫn cài đặt detail
- Configuration hướng dẫn
- Bảng phân loại nhiệt độ
- Troubleshooting
😊 → Đọc đầu tiên!
```

### 2. **SETUP.md** - Quick Start Guide
```
Hướng dẫn 5 bước:
1. Lấy Blynk Token (2 phút)
2. Tạo Telegram Bot (3 phút)
3. Cập nhật Config (2 phút)
4. Upload lên ESP8266 (3 phút)
5. Test & Sử dụng (5 phút)

Total: 15 phút để bắt đầu!
🚀 → Nhanh nhất để chạy ngay!
```

### 3. **BLYNK_SETUP.md** - Blynk Configuration Guide
```
Hướng dẫn chi tiết:
- Tải & cấu hình Blynk App
- Tạo 7 widgets (V0-V6)
- Dashboard layout
- Notifications & events
🎮 → Hướng dẫn Blynk chi tiết
```

### 4. **TELEGRAM_SETUP.md** - Telegram Bot Guide
```
Hướng dẫn chi tiết:
- Tạo bot với @BotFather
- Lấy Chat ID
- Tất cả commands
- Advanced features
- Security best practices
🤖 → Hướng dẫn Telegram chi tiết
```

### 5. **config.h** - Configuration Template
```cpp
Tất cả settings có thể cấu hình:
- WiFi SSID & Password
- Blynk Token & Virtual Pins
- Telegram Bot Token & Chat ID
- Pin definitions
- Temperature thresholds
- Update intervals
⚙️ → Tất cả config ở đây
```

---

## 🔌 Physical Connections

### Microcontroller: ESP8266 NodeMCU v2

```
┌─────────────────┐
│    ESP8266      │
│   NodeMCU v2    │
├─────────────────┤
│ GND ────────────┼─→ Ground (Chung)
│ 3.3V ───────────┼─→ Power (3.3V)
│
│ GPIO4 ──────────┼─→ DHT22 (Data pin)
│ GPIO13 (D7) ────┼─→ OLED (SDA - I2C)
│ GPIO12 (D6) ────┼─→ OLED (SCL - I2C)
│
│ GPIO15 (D8) ────┼─→ LED Green (xanh)
│ GPIO2 (D4) ─────┼─→ LED Yellow (vàng)
│ GPIO0 (D3) ─────┼─→ LED Red (đỏ)
│
│ GND ────────────┼─→ All GND
│ 3.3V ───────────┼─→ DHT22 + OLED
└─────────────────┘
```

---

## 💻 Software Architecture

### Main Loop Flow

```
setup()
  ├─ Initialize Serial
  ├─ Setup GPIO (LED)
  ├─ Initialize DHT22
  ├─ Initialize OLED Display
  ├─ Connect WiFi
  ├─ Initialize Telegram Bot
  └─ Connect Blynk
  
loop()
  ├─ Blynk.run() - handle Blynk messages
  │
  ├─ Every 2s: Read sensors
  │   ├─ DHT22.readTemperature()
  │   ├─ DHT22.readHumidity()
  │   ├─ Update display
  │   └─ Control LEDs
  │
  ├─ Every 5s: Send to Blynk
  │   ├─ Blynk.virtualWrite(V0, temp)
  │   ├─ Blynk.virtualWrite(V1, humidity)
  │   └─ Blynk.virtualWrite(V2, status)
  │
  └─ Every 1s: Check Telegram
      ├─ bot.getUpdates()
      └─ Process commands
```

### Virtual Pins Mapping

```
ESP8266 → Blynk Cloud → Blynk App
   │
   ├─ V0: Temperature (float)
   ├─ V1: Humidity (float)
   ├─ V2: Status (string)
   ├─ V3: All LEDs (button)
   ├─ V4: LED Green (button)
   ├─ V5: LED Yellow (button)
   └─ V6: LED Red (button)

ESP8266 → Telegram API → Telegram App
   │
   └─ Bidirectional messages
       ├─ Bot sends: /temp, /status, /humidity
       └─ Device sends: alerts, updates
```

---

## 🔄 Data Flow

### From Sensors to Cloud

```
Temperature/Humidity from DHT22
         ↓
Read every 2 seconds
         ↓
Parse temperature, humidity, status
         ↓
Display on OLED
         ↓
Control LEDs based on temperature
         ↓
Store in lastTemp, lastHumidity, lastStatus
         ↓
Every 5s: Send to Blynk
Every 1s: Check Telegram bot
         ↓
Blynk App & Telegram receive updates
```

### From Cloud to Device

```
User clicks button in Blynk App
         ↓
Signal sent to ESP8266 (Virtual Pin)
         ↓
BLYNK_WRITE() handler invoked
         ↓
digitalWrite() to control LED
         ↓
Confirmation sent back to Blynk App

User sends command to Telegram Bot
         ↓
bot.getUpdates() fetches message
         ↓
Message parsed and matched with /command
         ↓
Execute command (read temp, turn LED on, etc)
         ↓
bot.sendMessage() replies to user
```

---

## ⚙️ Configuration Quick Reference

### WiFi Configuration
```cpp
char ssid[] = "YOUR_SSID";              // Tên WiFi
char pass[] = "YOUR_PASSWORD";          // Mật khẩu
```

### Blynk Configuration
```cpp
char blynk_auth[] = "YOUR_BLYNK_TOKEN"; // Token từ email
```

### Telegram Configuration
```cpp
#define BOTtoken "BOT_TOKEN"            // Token từ @BotFather
#define CHAT_ID "CHAT_ID"               // ID từ @userinfobot
```

### Pin Configuration
```cpp
#define LED_GREEN  15    // GPIO15
#define LED_YELLOW 2     // GPIO2
#define LED_RED    0     // GPIO0
#define DHT_PIN    4     // GPIO4
#define OLED_SDA   13    // GPIO13
#define OLED_SCL   12    // GPIO12
```

### Temperature Thresholds
```cpp
#define TEMP_COLD_THRESHOLD 13       // LED Green
#define TEMP_COOL_THRESHOLD 25       // LED Yellow
#define TEMP_HOT_THRESHOLD 35        // LED Red
```

---

## 📊 Temperature Status Table

| Range | Status | LED | Description |
|---|---|:---:|---|
| < 13°C | TOO COLD | 🟢 Green | Quá lạnh |
| 13-20°C | COLD | 🟢 Green | Lạnh |
| 20-25°C | COOL | 🟡 Yellow | Mát |
| 25-30°C | WARM | 🟡 Yellow | Ấm |
| 30-35°C | HOT | 🔴 Red | Nóng |
| ≥ 35°C | TOO HOT | 🔴 Red | Quá nóng |

---

## 🚀 Getting Started Checklist

### Before Upload
```
☐ Cài đặt PlatformIO trong VS Code
☐ Tạo Blynk account & lấy token
☐ Tạo Telegram bot & lấy token + chat ID
☐ Cập nhật WiFi SSID & Password
☐ Cập nhật Blynk token trong code
☐ Cập nhật Telegram token & chat ID
☐ Kết nối ESP8266 qua USB
☐ Chọn correct COM port
```

### After Upload
```
☐ Mở Serial Monitor (115200 baud)
☐ Xem "WiFi connected" message
☐ Xem "Blynk connected" message
☐ Kiểm tra OLED display
☐ Kiểm tra LED nhấp nháy
☐ Test Blynk App buttons
☐ Test Telegram commands (/temp, /status)
☐ Xem dữ liệu cập nhật real-time
```

---

## 📞 Key Contacts for Issues

### WiFi Not Connected
```
→ Check SSID & Password
→ Check WiFi signal strength
→ Check ESP8266 not too far from router
→ Try WiFi Manager: https://github.com/tzapu/WiFiManager
```

### Blynk Not Connected
```
→ Check token is correct (no extra spaces)
→ Check internet connection
→ Check Blynk server: https://status.blynk.io
→ Try on different WiFi
```

### Telegram Bot Not Responding
```
→ Check Token is correct
→ Check Chat ID is correct
→ Send /start to bot first
→ Check WiFi connection (Serial says "0" ms intervals)
→ Try deleting bot and creating new one
```

### Sensors Not Reading
```
→ Check DHT22 wiring
→ Check I2C address of OLED (0x3C vs 0x3D)
→ Use I2C scanner
→ Try 3.3V or 5V (DHT22 works both)
```

---

## 🔗 Useful Resources

| Resource | Link |
|---|---|
| PlatformIO | https://platformio.org |
| Arduino ESP8266 | https://github.com/esp8266/Arduino |
| Blynk Docs | https://docs.blynk.io |
| Telegram Bot API | https://core.telegram.org/bots/api |
| DHT22 Library | https://github.com/adafruit/DHT-sensor-library |
| SSD1306 Library | https://github.com/adafruit/Adafruit_SSD1306 |

---

## 👥 Team Members

| # | Name | Role |
|---|---|---|
| 1 | Lê Công Mẫn | - |
| 2 | Võ Tiến Đạt | - |
| 3 | Hồ Thị Kiều Oanh | - |
| 4 | Nguyễn Phan Thủy Tiên | - |

---

## 📅 Project Timeline

| Phase | Duration | Status |
|---|:---:|:---:|
| Planning | 1 week | ✅ Complete |
| Hardware Assembly | 1 week | ✅ Complete |
| Software Development | 2 weeks | ✅ Complete |
| Testing & Debugging | 1 week | ✅ Complete |
| Documentation | 1 week | ✅ Complete |

**Total:** 15 days | **Status:** Ready for Production ✅

---

## 📄 File Versions

| File | Version | Last Updated |
|---|:---:|---|
| main.cpp | 1.0.0 | Mar 24, 2026 |
| README.md | 1.0.0 | Mar 24, 2026 |
| SETUP.md | 1.0.0 | Mar 24, 2026 |
| BLYNK_SETUP.md | 1.0.0 | Mar 24, 2026 |
| TELEGRAM_SETUP.md | 1.0.0 | Mar 24, 2026 |
| config.h | 1.0.0 | Mar 24, 2026 |

---

## 🎓 Learning Outcomes

After completing this project, you will understand:

- ✅ IoT platform integration (Blynk)
- ✅ Telegram Bot API usage
- ✅ DHT22 sensor interfacing
- ✅ OLED SSD1306 display control
- ✅ Non-blocking code patterns
- ✅ Virtual pins and real-time data
- ✅ Multi-protocol communication
- ✅ WiFi connectivity on ESP8266
- ✅ LED control and PWM
- ✅ Serial debugging techniques

---

## 🏆 Achievement Unlocked!

```
┌──────────────────────────────────────┐
│  ★ ★ ★ IoT Project Complete ★ ★ ★   │
├──────────────────────────────────────┤
│                                      │
│  You have successfully created:      │
│                                      │
│  ✅ A fully functional IoT system    │
│  ✅ With cloud integration (Blynk)   │
│  ✅ With Telegram Bot control        │
│  ✅ With real-time monitoring        │
│  ✅ With comprehensive docs          │
│                                      │
│  Level: Professional IoT Developer   │
│  XP gained: +9000                    │
│                                      │
└──────────────────────────────────────┘
```

---

## 📞 Support & Questions

- 📖 Read README.md first
- 🔍 Check SETUP.md for quick troubleshooting
- 💬 Join Blynk Community: https://community.blynk.cc/
- 🤖 Check UniversalTelegramBot repo issues
- 📺 Search YouTube for similar projects

---

## 📝 License

```
MIT License

Copyright (c) 2026 Group 7

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction...
```

---

**Project by Group 7 | Updated: March 24, 2026 | Version: 1.0.0**

**Good Luck! 🚀**
