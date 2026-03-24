# 🚀 HƯỚNG DẪN CÀI ĐẶT NHANH (Quick Start)

## ⚡ 5 Bước để bắt đầu

### Bước 1️⃣: Lấy Blynk Token (2 phút)

```
📱 Trên điện thoại:
1. Tải app "Blynk" (iOS: App Store, Android: Google Play)
2. Đăng ký tài khoản
3. Tạo Project mới → Chọn "ESP8266"
4. Token sẽ được⤙ qua email hoặc hiện trên app
5. Copy token (ví dụ: abc123xyz456...)
```

**Kết quả:** Có token dạng `abc123xyz456...`

---

### Bước 2️⃣: Tạo Telegram Bot (3 phút)

```
💬 Trên Telegram:

A) Lấy Bot Token:
   1. Mở Telegram, tìm @BotFather
   2. Gõ /start → /newbot
   3. Đặt tên: "Group7-Monitor"
   4. Đặt username: "group7_monitor_bot"
   5. Lấy token (ví dụ: 123456:ABCDEf_xyz...)

B) Lấy Chat ID:
   1. Tìm @userinfobot
   2. Gõ /start
   3. Nhận được Chat ID (ví dụ: 987654321)

C) Bắt đầu bot:
   1. Tìm bot vừa tạo: @group7_monitor_bot
   2. Gõ /start
```

**Kết quả:** Bot Token + Chat ID

---

### Bước 3️⃣: Cập nhật Config (2 phút)

**Mở file `src/main.cpp` và tìm phần này:**

```cpp
// ============= CẤU HÌNH WIFI =============
char ssid[] = "YOUR_SSID";              // ← Thay bằng tên WiFi
char pass[] = "YOUR_PASSWORD";          // ← Thay bằng mật khẩu

// ============= CẤU HÌNH BLYNK =============
char blynk_auth[] = "YOUR_BLYNK_AUTH_TOKEN";  // ← Dán token Blynk

// ============= CẤU HÌNH TELEGRAM =============
#define BOTtoken "YOUR_BOT_TOKEN"       // ← Dán token Bot
#define CHAT_ID "YOUR_CHAT_ID"          // ← Dán Chat ID
```

**Ví dụ sau khi cập nhật:**
```cpp
char ssid[] = "MyWiFi";
char pass[] = "password123";
char blynk_auth[] = "abc123xyz456def...";
#define BOTtoken "123456:ABCDEf_xyz..."
#define CHAT_ID "987654321"
```

---

### Bước 4️⃣: Upload lên ESP8266 (3 phút)

#### Cách A: VS Code + PlatformIO (Khuyến nghị)

```bash
# Bước 1: Kết nối USB
# Bước 2: Chọn COM port tại dưới cùng (esp8266)
# Bước 3: Ấn nút Upload (→)

# Hoặc gõ lệnh trong Terminal:
pio run --target upload
```

#### Cách B: Arduino IDE

```
1. Tools → Board → NodeMCU 1.0 (ESP8266-12E)
2. Tools → Port → COM (của bạn)
3. Sketch → Upload
```

---

### Bước 5️⃣: Test & Sử dụng (5 phút)

#### 📋 Kiểm tra Serial Monitor

```
1. Mở Serial Monitor (baud 115200)
2. Xem dòng chữ:
   - "Bắt đầu khởi tạo hệ thống..."
   - "WiFi đã kết nối!"
   - "Blynk đã kết nối!"
3. Nếu có lỗi → xem phần Troubleshooting
```

#### 📲 Test Blynk App

```
1. Mở Blynk app
2. Xem dữ liệu cập nhật:
   - V0: Nhiệt độ
   - V1: Độ ẩm
   - V2: Trạng thái
3. Ấn nút bật V3 để kiểm tra LED
```

#### 🤖 Test Telegram Bot

```
1. Mở Telegram
2. Tìm bot: @group7_monitor_bot (hoặc tên bạn đặt)
3. Gõ các lệnh:
   - /start → Xem menu
   - /temp → Hiển thị nhiệt độ
   - /status → Xem trạng thái
```

**Bạn đã hoàn tất! ✅**

---

## 🔍 Troubleshooting Nhanh

### ❌ Không kết nối WiFi
```
✓ Kiểm tra SSID & Password có khớp không
✓ Kiểm tra Router có bật không
✓ Thử restart ESP8266
✓ Kiểm tra khoảng cách
```

### ❌ Blynk không hiển thị dữ liệu
```
✓ Kiểm tra được kết nối WiFi (Serial Monitor)
✓ Kiểm tra Blynk Token chính xác
✓ Kiểm tra kết nối Internet
✓ Khởi động lại app Blynk
```

### ❌ Telegram Bot không phản ứng
```
✓ Kiểm tra kết nối WiFi
✓ Kiểm tra Bot Token chính xác
✓ Kiểm tra Chat ID chính xác
✓ Kiểm tra đã gõ /start cho bot
```

### ❌ OLED không hiển thị
```
✓ Kiểm tra dây nối VCC/GND/SDA/SCL
✓ Thử thay câu lệnh: 0x3C → 0x3D
✓ Kiểm tra I2C scan: https://gist.github.com/...
```

### ❌ DHT22 lỗi: "Failed to read DHT"
```
✓ Kiểm tra dây nối chắc chắn
✓ Thử disconnect USB 5s rồi kết nối lại
✓ Kiểm tra pin GPIO4 có bị chiếm không
```

---

## 📊 Giám sát dữ liệu

### Cách 1: OLED Display (Trực tiếp)
- Xem nhiệt độ, độ ẩm, trạng thái
- Xem trạng thái WiFi & Blynk

### Cách 2: Blynk App (Trên điện thoại)
- Theo dõi real-time từ mọi nơi
- Điều khiển LED từ xa

### Cách 3: Telegram Bot (Nhận tin nhắn)
- Gửi lệnh `/temp` để kiểm tra
- Nhận cảnh báo tự động

### Cách 4: Serial Monitor (Debug)
- Xem log chi tiết
- Kiểm tra lỗi

---

## 🔧 Điều chỉnh Advanced

### Thay đổi Thời gian cập nhật
**File: `src/main.cpp`**
```cpp
const unsigned long TEMP_READ_INTERVAL = 2000;     // Thay 2000 → 5000 (thành 5s)
const unsigned long BLYNK_SEND_INTERVAL = 5000;    // Thay 5000 → 10000 (thành 10s)
```

### Thay đổi Ngưỡng nhiệt độ
**File: `src/main.cpp`**
```cpp
#define TEMP_COLD_THRESHOLD 13    // Thay 13 → tùy ý
#define TEMP_WARM_THRESHOLD 25    // Thay 25 → tùy ý
#define TEMP_HOT_THRESHOLD 35     // Thay 35 → tùy ý
```

### Bật/Tắt Telegram
**File: `src/main.cpp`**
```cpp
// Mở comment dòng này để tắt Telegram:
// checkTelegramMessages();
```

---

## 📞 Hỗ trợ & Liên hệ

- **Lỗi PlatformIO:** Xóa `.pio` folder và upload lại
- **Lỗi USB:** Cài driver CP2102 hoặc CH340
- **Mất kết nối:** Kiểm tra mã lỗi Serial Monitor

---

## ✅ Danh sách kiểm tra trước khi upload

- [ ] Đã thay WiFi SSID
- [ ] Đã thay WiFi Password
- [ ] Đã thay Blynk Token
- [ ] Đã thay Bot Token Telegram
- [ ] Đã thay Chat ID Telegram
- [ ] Đã chọn board ESP8266
- [ ] Đã chọn COM port đúng
- [ ] Đã kết nối USB

---

**Chúc bạn thành công! 🎉**

Ver: 1.0 | Date: March 2026 | Group 7
