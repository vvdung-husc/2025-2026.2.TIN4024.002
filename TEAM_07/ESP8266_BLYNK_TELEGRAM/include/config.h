/*
═══════════════════════════════════════════════════════════════
CONFIGURATION TEMPLATE - Nhóm 7 IoT Project
═══════════════════════════════════════════════════════════════

Hướng dẫn:
1. Sao chép nội dung này vào file config.h trong thư mục include/
2. Cập nhật các giá trị YOUR_* bằng thông tin của bạn
3. Include file này trong main.cpp
═══════════════════════════════════════════════════════════════
*/

#ifndef CONFIG_H
#define CONFIG_H

// ╔════════════════════════════════════════════════════════════╗
// ║          1. CẤU HÌNH WiFi NETWORK                          ║
// ╚════════════════════════════════════════════════════════════╝

#define WIFI_SSID "YOUR_SSID"                    // Tên WiFi
#define WIFI_PASSWORD "YOUR_PASSWORD"            // Mật khẩu WiFi

// Note: Hỗ trợ cả DHCPù và IP tĩnh
// Để dùng IP tĩnh, bỏ comment các dòng dưới:
/*
#define STATIC_IP
#define IP_ADDRESS "192.168.1.100"
#define GATEWAY "192.168.1.1"
#define SUBNET "255.255.255.0"
#define DNS "192.168.1.1"
*/

// ╔════════════════════════════════════════════════════════════╗
// ║          2. CẤU HÌNH BLYNK IoT PLATFORM                    ║
// ╚════════════════════════════════════════════════════════════╝

#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

// BLYNK Server (có thể dùng server khác)
#define BLYNK_SERVER "blynk.cloud"
#define BLYNK_PORT 80

// Tùy chọn: Dùng template
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "Group7-IoT-Monitor"

// ╔════════════════════════════════════════════════════════════╗
// ║          3. CẤU HÌNH TELEGRAM BOT                          ║
// ╚════════════════════════════════════════════════════════════╝

#define TELEGRAM_BOT_TOKEN "YOUR_BOT_TOKEN"      // Token từ BotFather
#define TELEGRAM_CHAT_ID "YOUR_CHAT_ID"          // Chat ID của bạn

// Tùy chọn: Bật/Tắt tính năng Telegram
#define ENABLE_TELEGRAM_BOT true
#define TELEGRAM_UPDATE_INTERVAL 1000            // Kiểm tra tin nhắn mỗi 1000ms

// ╔════════════════════════════════════════════════════════════╗
// ║          4. CẤU HÌNH PHẦN CỨNG - PINOUT                    ║
// ╚════════════════════════════════════════════════════════════╝

// I2C - OLED Display
#define OLED_SDA 13                              // GPIO13 - SDA
#define OLED_SCL 12                              // GPIO12 - SCL

// DHT22 Sensor
#define DHT_PIN 4                                // GPIO4 - Data

// LED Pins
#define LED_GREEN 15                             // GPIO15 - LED Xanh (COLD)
#define LED_YELLOW 2                             // GPIO2 - LED Vàng (WARM)
#define LED_RED 0                                // GPIO0 - LED Đỏ (HOT)

// Button pins (tùy chọn)
#define BUTTON_PIN 5                             // GPIO5 - Button
#define BUZZER_PIN 14                            // GPIO14 - Buzzer (tùy chọn)

// ╔════════════════════════════════════════════════════════════╗
// ║          5. CẤU HÌNH DISPLAY OLED                          ║
// ╚════════════════════════════════════════════════════════════╝

#define SCREEN_WIDTH 128                         // Chiều rộng pixels
#define SCREEN_HEIGHT 64                         // Chiều cao pixels
#define OLED_I2C_ADDRESS 0x3C                    // Địa chỉ I2C (0x3C hoặc 0x3D)

// ╔════════════════════════════════════════════════════════════╗
// ║          6. CẤU HÌNH CẢM BIẾN DHT22                        ║
// ╚════════════════════════════════════════════════════════════╝

#define DHT_TYPE DHT22                           // Loại cảm biến
#define DHT_READ_INTERVAL 2000                   // Đọc mỗi 2000ms (2s)

// Compensate cho độ lệch
#define TEMP_OFFSET 0.0                          // Bù độ lệch nhiệt độ
#define HUMIDITY_OFFSET 0.0                      // Bù độ lệch độ ẩm

// ╔════════════════════════════════════════════════════════════╗
// ║          7. NGƯỠNG NHIỆT ĐỘ VÀ CẢNH BÁO                    ║
// ╚════════════════════════════════════════════════════════════╝

#define TEMP_TOO_COLD 13                         // Quá lạnh
#define TEMP_COLD 20                             // Lạnh
#define TEMP_COOL 25                             // Mát
#define TEMP_WARM 30                             // Ấm
#define TEMP_HOT 35                              // Nóng

// Cảnh báo Telegram
#define TEMP_ALERT_TOO_HOT 40                    // Gửi cảnh báo khi > 40°C
#define TEMP_ALERT_TOO_COLD 5                    // Gửi cảnh báo khi < 5°C
#define ENABLE_TEMP_ALERTS true                  // Bật/Tắt cảnh báo

// ╔════════════════════════════════════════════════════════════╗
// ║          8. THỜI GIAN CẬP NHẬT                             ║
// ╚════════════════════════════════════════════════════════════╝

#define SENSOR_READ_INTERVAL 2000                // Đọc cảm biến (2s)
#define BLYNK_SEND_INTERVAL 5000                 // Gửi Blynk (5s)
#define DISPLAY_UPDATE_INTERVAL 1000             // Cập nhật hiển thị (1s)
#define TELEGRAM_CHECK_INTERVAL 1000             // Kiểm tra Telegram (1s)

// ╔════════════════════════════════════════════════════════════╗
// ║          9. VIRTUAL PINS BLYNK                             ║
// ╚════════════════════════════════════════════════════════════╝

#define VPIN_TEMPERATURE V0                      // Nhập nhiệt độ
#define VPIN_HUMIDITY V1                         // Nhập độ ẩm
#define VPIN_STATUS V2                           // Hiển thị trạng thái
#define VPIN_LED_ALL V3                          // Button bật/tắt tất cả
#define VPIN_LED_GREEN V4                        // Button LED xanh
#define VPIN_LED_YELLOW V5                       // Button LED vàng
#define VPIN_LED_RED V6                          // Button LED đỏ

// ╔════════════════════════════════════════════════════════════╗
// ║          10. TÙY CHỌN KHÁC                                 ║
// ╚════════════════════════════════════════════════════════════╝

#define ENABLE_SERIAL_DEBUG true                 // Bật debug qua Serial
#define SERIAL_BAUD 115200                       // Tốc độ Serial

#define ENABLE_OTA_UPDATE false                  // Over-The-Air Update (tùy chọn)

#define DEVICE_LOCATION "Laboratory Room 7"      // Vị trí thiết bị
#define DEVICE_VERSION "1.0.0"                   // Phiên bản

#endif // CONFIG_H

/*
═══════════════════════════════════════════════════════════════
HƯỚNG DẪN LẤY THÔNG TIN CỰC KỲ QUAN TRỌNG
═══════════════════════════════════════════════════════════════

1️⃣  BLYNK AUTH TOKEN:
   - Tải app Blynk trên điện thoại
   - Tạo dự án mới
   - Chọn board ESP8266
   - Token sẽ được gửi qua email

2️⃣  TELEGRAM BOT TOKEN & CHAT ID:
   - Mở Telegram, tìm @BotFather
   - Gõ /start → /newbot
   - Đặt tên và username bot
   - Lấy token được cấp
   
   - Tìm @userinfobot
   - Gởi /start để lấy Chat ID

3️⃣  WiFi SSID & PASSWORD:
   - Tên chính là tên WiFi hiển thị
   - Password là mật khẩu WiFi của bạn

═══════════════════════════════════════════════════════════════
*/
