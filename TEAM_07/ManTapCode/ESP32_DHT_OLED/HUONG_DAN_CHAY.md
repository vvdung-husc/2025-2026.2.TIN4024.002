## Hướng dẫn sửa lỗi OLED ESP32 DHT22

### Vấn đề phổ biến:
1. **OLED không khởi tạo** - I2C chưa sẵn sàng
2. **Kết nối sai** - GPIO không đúng
3. **Thứ tự khởi tạo** - Wire phải khởi tạo trước display.begin()

### Các sửa chữa đã áp dụng:
- Thêm delay 500ms sau Wire.begin()
- Thêm delay 200ms sau Wire.setClock()
- Đơn giản hoá ký tự để tránh lỗi encoding

### Chạy trên Wokwi Simulator:
1. Mở VS Code
2. Cài đặt extension "Wokwi for VS Code"
3. Nhấn Ctrl+Shift+P → "Wokwi: Start Simulator"
4. Hoặc nhấn F1 → chọn "Wokwi Start"

### Kiểm tra kết nối trong Wokwi:
- SDA: GPIO 21 ✓
- SCL: GPIO 22 ✓
- OLED I2C Address: 0x3c ✓
- DHT Data: GPIO 15 ✓
- LED Green: GPIO 5 ✓
- LED Yellow: GPIO 18 ✓
- LED Red: GPIO 19 ✓

### Nếu vẫn không chạy được:
1. Nhấn F1 → "Wokwi: Rebuild"
2. Hoặc delete thư mục .pio và build lại
3. Kiểm tra Terminal để xem error messages
