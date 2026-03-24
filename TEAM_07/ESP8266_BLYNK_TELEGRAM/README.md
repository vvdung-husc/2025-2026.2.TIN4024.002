# Hệ thống Giám sát Nhiệt độ và Độ ẩm - Nhóm 7

## 📋 Thông tin dự án

**Tên dự án:** Hệ thống IoT giám sát nhiệt độ và độ ẩm với Blynk & Telegram Bot

**Các thành viên nhóm:**
1. Lê Công Mẫn
2. Võ Tiến Đạt
3. Hồ Thị Kiều Oanh
4. Nguyễn Phan Thủy Tiên

**Mục tiêu:** 
- Đọc nhiệt độ và độ ẩm từ cảm biến DHT22
- Hiển thị dữ liệu lên màn hình OLED SSD1306
- Điều khiển 3 đèn LED dựa vào nhiệt độ
- Gửi dữ liệu lên nền tảng IoT **Blynk**
- Tích hợp **Telegram Bot** để nhận cảnh báo và điều khiển từ xa

---

## ⚙️ Thiết bị sử dụng

### Microcontroller
- **ESP8266 NodeMCU v2** (hoặc ESP32)

### Cảm biến
- **DHT22** - Cảm biến nhiệt độ và độ ẩm

### Hiển thị
- **SSD1306 OLED Display** (128x64) - Giao diện I2C

### Đèn LED
- **LED Xanh (Cyan)** - Trạng thái lạnh
- **LED Vàng** - Trạng thái trung bình
- **LED Đỏ** - Trạng thái nóng

### Điện trở bảo vệ
- **2x Resistor** (220Ω, 330Ω)

---

## 📌 Sơ đồ kết nối

```
ESP8266 NodeMCU
├── GPIO15 ──→ LED Cyan (xanh)
├── GPIO2  ──→ LED Yellow (vàng)
├── GPIO0  ──→ LED Red (đỏ)
├── GPIO4  ──→ DHT22 (Data)
├── GPIO13 ──→ OLED SDA (I2C)
├── GPIO12 ──→ OLED SCL (I2C)
├── 3.3V   ──→ OLED VCC + DHT22 VCC
└── GND    ──→ OLED GND + DHT22 GND
```

---

## 🔧 Cài đặt và Configuration

### 1. Chuẩn bị

**Các công cụ cần thiết:**
- VS Code + PlatformIO Extension
- Arduino IDE (tùy chọn)
- Blynk App / Web Dashboard
- Telegram (để tạo và điều khiển Bot)

### 2. Cấu hình chính trong `main.cpp`

Mở file `src/main.cpp` và cập nhật các thông tin sau:

#### WiFi Configuration
```cpp
char ssid[] = "YOUR_SSID";              // Tên WiFi
char pass[] = "YOUR_PASSWORD";          // Mật khẩu WiFi
```

#### Blynk Configuration
```cpp
char blynk_auth[] = "YOUR_BLYNK_AUTH_TOKEN";  // Token từ Blynk
```

#### Telegram Bot Configuration
```cpp
#define BOTtoken "YOUR_BOT_TOKEN"       // Token từ BotFather
#define CHAT_ID "YOUR_CHAT_ID"          // Chat ID của bạn
```

### 3. Làm thế nào để lấy các Token?

#### a) Blynk Token
1. Tải ứng dụng Blynk từ App Store hoặc Google Play
2. Tạo tài khoản mới
3. Tạo Project mới
4. Chọn "ESP8266" làm Device
5. Token sẽ được gửi qua email hoặc hiển thị trong app

#### b) Telegram Bot Token
1. Mở Telegram, tìm kiếm "BotFather"
2. Gõ `/start` rồi `/newbot`
3. Đặt tên bot và username
4. Lấy token được cấp
5. Tìm Chat ID:
   - Gõ `@userinfobot` trong Telegram
   - Gửi `/start` sẽ nhận được Chat ID

### 4. Upload mã

**Cách 1: PlatformIO (Khuyến nghị)**
```bash
# Terminal trong VS Code
pio run --target upload
```

**Cách 2: Arduino IDE**
```bash
# Chọn Board: NodeMCU 1.0 (ESP8266-12E Module)
# Chọn COM Port và upload
```

---

## 📊 Virtual Pins Blynk

| Virtual Pin | Chức năng | Mô tả |
|---|---|---|
| **V0** | Display | Hiển thị Nhiệt độ hiện tại |
| **V1** | Display | Hiển thị Độ ẩm hiện tại |
| **V2** | Display | Hiển thị Trạng thái (COLD, HOT, etc) |
| **V3** | Button | Bật/Tắt tất cả LED |
| **V4** | Button | Điều khiển LED Xanh |
| **V5** | Button | Điều khiển LED Vàng |
| **V6** | Button | Điều khiển LED Đỏ |

---

## 💬 Telegram Bot Commands

| Lệnh | Tác dụng |
|---|---|
| `/start` | Hiển thị menu trợ giúp |
| `/temp` | Lấy nhiệt độ hiện tại |
| `/humidity` | Lấy độ ẩm hiện tại |
| `/status` | Xem trạng thái đầy đủ |
| `/ledon` | Bật LED |
| `/ledoff` | Tắt LED |

---

## 🌡️ Bảng phân loại Nhiệt độ

| Khoảng Nhiệt độ | Trạng thái | LED |
|---|---|---|
| < 13°C | TOO COLD | 🟢 Xanh |
| 13-20°C | COLD | 🟢 Xanh |
| 20-25°C | COOL | 🟡 Vàng |
| 25-30°C | WARM | 🟡 Vàng |
| 30-35°C | HOT | 🔴 Đỏ |
| ≥ 35°C | TOO HOT | 🔴 Đỏ |

---

## 📱 Tính năng chính

### ✅ Đã thực hiện
1. ✓ Đọc dữ liệu từ DHT22
2. ✓ Hiển thị trên OLED SSD1306
3. ✓ Điều khiển 3 đèn LED tự động
4. ✓ Kết nối WiFi
5. ✓ Gửi dữ liệu lên Blynk
6. ✓ Tích hợp Telegram Bot
7. ✓ Điều khiển LED từ Telegram
8. ✓ Điều khiển từ Blynk App

### 📝 Có thể mở rộng thêm
- [ ] Ghi dữ liệu vào EEPROM
- [ ] Cảnh báo tự động qua Telegram (SNS)
- [ ] Điều chỉnh ngưỡng nhiệt độ qua Telegram
- [ ] Lịch sử dữ liệu trên Cloud
- [ ] Thêm cảm biến áp suất
- [ ] Thêm cảm biến ánh sáng

---

## 🚀 Cách sử dụng

### 1. Lần đầu tiên
1. Cập nhật WiFi, Blynk Token, và Telegram Bot Info
2. Upload mã lên ESP8266
3. Mở Serial Monitor (115200 baud) để xem lỗi
4. Mở Blynk App để xem dữ liệu
5. Gửi `/start` cho Telegram Bot

### 2. Theo dõi dữ liệu
- **OLED Display** - Hiển thị real-time
- **Serial Monitor** - Debug output
- **Blynk App** - Giám sát và điều khiển từ xa
- **Telegram** - Nhận cảnh báo tức thì

### 3. Điều khiển
- Bật/Tắt LED bằng Blynk App
- Kiểm tra trạng thái bằng Telegram commands
- Đèn sẽ tự động chuyển màu theo nhiệt độ

---

## 🐛 Troubleshooting

### Problem: OLED không hiển thị
- Kiểm tra chân I2C (SDA=GPIO13, SCL=GPIO12)
- Kiểm tra địa chỉ I2C (mặc định 0x3C)
- Kiểm tra dây nối VCC/GND

### Problem: DHT22 không đọc được
- Kiểm tra chân Data (GPIO4)
- Kiểm tra dây nối
- Thử restart ESP8266

### Problem: Không kết nối WiFi
- Kiểm tra SSID và Password
- Kiểm tra khoảng cách tới Router
- Khởi động lại Router

### Problem: Blynk không kết nối
- Kiểm tra WiFi kết nối
- Kiểm tra Blynk Token
- Kiểm tra server Blynk có bị tắt không

### Problem: Telegram Bot không phản ứng
- Kiểm tra Bot Token
- Kiểm tra Chat ID
- Đảm bảo WiFi kết nối tốt

---

## 📚 Thư viện được sử dụng

```ini
[lib_deps]
https://github.com/avishorp/TM1637.git
blynkkk/Blynk @ ^1.3.2
adafruit/DHT sensor library @ ^1.4.6
adafruit/Adafruit Unified Sensor @ ^1.1.14
adafruit/Adafruit GFX Library @ ^1.11.0
adafruit/Adafruit SSD1306 @ ^2.5.7
witnessmenow/UniversalTelegramBot @ ^1.3.0
bblanchon/ArduinoJson @ ^6.21.3
olikraus/U8g2 @ ^2.35.0
```

---

## 📖 Tài liệu tham khảo

- [Blynk Documentation](https://docs.blynk.io/)
- [Telegram Bot API](https://core.telegram.org/bots/api)
- [Adafruit DHT22](https://learn.adafruit.com/dht)
- [Adafruit SSD1306](https://learn.adafruit.com/monochrome-oled-displays-with-arduino)
- [ESP8266 Pinout](https://github.com/esp8266/Arduino/wiki/Pin-definitions)

---

## 📄 Giấy phép

Dự án này được phát hành dưới giấy phép MIT. Bạn có thể sử dụng, sửa đổi và phân phối mã nguồn.

---

## 👥 Liên hệ

Nếu có câu hỏi hoặc góp ý, vui lòng liên hệ:
- **Email:** team07@university.edu
- **GitHub:** [Link GitHub](https://github.com/yourusername)

---

**Cập nhật lần cuối:** March 2026
**Phiên bản:** 1.0.0
