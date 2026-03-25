# Bài tập IoT - Lê Trọng Kha (22t1020634)

## Thông tin sinh viên
- Họ và tên: Lê Trọng Kha
- MSSV: 22t1020634
- Nhóm: TEAM_01

## Danh sách các dự án

### 1. LED_Blink
Dự án cơ bản về nhấp nháy LED với ESP32

### 2. ESP32_DHT_OLED
Hệ thống giám sát nhiệt độ và độ ẩm với:
- Cảm biến DHT22
- Màn hình OLED SSD1306
- 3 LED cảnh báo theo ngưỡng nhiệt độ
- Hiển thị thông tin nhiệt độ, độ ẩm và trạng thái

### 3. Traffic_Light
Hệ thống đèn giao thông thông minh với:
- 3 LED (Đỏ, Vàng, Xanh)
- Màn hình 7 đoạn TM1637 hiển thị đếm ngược
- Nút nhấn bật/tắt hiển thị
- Cảm biến ánh sáng LDR tự động bật/tắt đèn
- LED xanh dương làm đèn đường (tự động bật khi tối)

### 4. API_TELEGRAM
Điều khiển LED qua Telegram Bot với:
- Cảm biến chuyển động PIR
- LED điều khiển từ xa
- Các lệnh: /start, /led_on, /led_off, /get_state
- Tự động gửi cảnh báo khi phát hiện chuyển động

## Công nghệ sử dụng
- Board: ESP32 DevKit V4
- Framework: Arduino
- Platform: PlatformIO
- Simulator: Wokwi

## Thư viện
- Adafruit SSD1306
- Adafruit GFX Library
- DHT sensor library
- TM1637Display
- UniversalTelegramBot
- ArduinoJson
