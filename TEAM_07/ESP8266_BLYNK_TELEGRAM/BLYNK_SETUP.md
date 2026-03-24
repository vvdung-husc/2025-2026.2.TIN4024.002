# 🎮 Hướng dẫn cấu hình Blynk App

## 📱 Cài đặt Blynk App

### Bước 1: Tải ứng dụng
- **iOS:** [App Store - Blynk IOT](https://apps.apple.com/us/app/blynk-iot-home-automation/id907674537)
- **Android:** [Google Play - Blynk IOT](https://play.google.com/store/apps/details?id=cloud.blynk)

### Bước 2: Tạo tài khoản
1. Mở app Blynk
2. Chọn "Sign Up"
3. Nhập email và password
4. Xác nhận email
5. Đăng nhập

### Bước 3: Tạo Project
```
1. Ấn nút "New Project" (+)
2. Đặt tên: "Group7-IoT-Monitor"
3. Chọn Device: "ESP8266"
4. Chọn Connection: "WiFi"
5. Ấn "Create"
6. Token sẽ được gửi qua email
```

---

## 🔌 Cấu hình Virtual Pins (Widgets)

### Dashboard Layout

Sau khi tạo project, vào Edit mode (ấn biểu tượng pencil) và thêm widgets:

```
┌─────────────────────────────────────────┐
│         NHOM 7 - IoT Monitor            │
├─────────────────────────────────────────┤
│  [V0] 温度: 25.5°C    [V1] 湿度: 60%   │
├─────────────────────────────────────────┤
│  Status: WARM                            │
├─────────────────────────────────────────┤
│  [●] All LED   [●] Green [●] Yellow    │
├─────────────────────────────────────────┤
│  [●] Red       WiFi: Connected          │
└─────────────────────────────────────────┘
```

---

## 📍 Chi tiết từng Widget

### 1. Widget hiển thị Nhiệt độ (V0)

```
Type: Value Display
├─ Virtual Pin: V0
├─ Name: "Temperature"
├─ Color: Orange/Red
├─ Text Size: Large
├─ Display format: 2 decimal places
└─ Unit: °C
```

**Cách thêm:**
1. Ấn "+" → Search "Value Display"
2. Tap vào widget
3. Cấu hình như trên
4. Chọn V0
5. Done

---

### 2. Widget hiển thị Độ ẩm (V1)

```
Type: Value Display
├─ Virtual Pin: V1
├─ Name: "Humidity"
├─ Color: Blue
├─ Text Size: Large
├─ Display format: 2 decimal places
└─ Unit: %
```

---

### 3. Widget hiển thị Trạng thái (V2)

```
Type: Text/Label Display
├─ Virtual Pin: V2
├─ Name: "Status"
├─ Color: Green
└─ Text Size: Medium
```

---

### 4. Button Điều khiển Tất cả LED (V3)

```
Type: Button
├─ Virtual Pin: V3
├─ Name: "All LEDs"
├─ Mode: SWITCH (ON/OFF)
├─ ON: Send 1
├─ OFF: Send 0
├─ Button Color: Purple
└─ Text: "All LEDs"
```

**Cách sử dụng:**
- Tắt: Button mở
- Bật: Button đóng

---

### 5. Button Điều khiển LED xanh (V4)

```
Type: Button
├─ Virtual Pin: V4
├─ Name: "LED Green"
├─ Mode: SWITCH (ON/OFF)
├─ ON: Send 1
├─ OFF: Send 0
├─ Button Color: Cyan
└─ Text: "Green"
```

---

### 6. Button Điều khiển LED vàng (V5)

```
Type: Button
├─ Virtual Pin: V5
├─ Name: "LED Yellow"
├─ Mode: SWITCH (ON/OFF)
├─ ON: Send 1
├─ OFF: Send 0
├─ Button Color: Yellow
└─ Text: "Yellow"
```

---

### 7. Button Điều khiển LED đỏ (V6)

```
Type: Button
├─ Virtual Pin: V6
├─ Name: "LED Red"
├─ Mode: SWITCH (ON/OFF)
├─ ON: Send 1
├─ OFF: Send 0
├─ Button Color: Red
└─ Text: "Red"
```

---

## 📊 Widget tùy chọn (Optional)

### 8. Biểu đồ Nhiệt độ theo thời gian

```
Type: Superchart / Timeline
├─ Virtual Pin: V0
├─ Chart Name: "Temperature Graph"
├─ Y-axis Label: "Temperature (°C)"
├─ X-axis Label: "Time"
├─ Color: Orange
└─ Update Interval: 5 seconds
```

---

### 9. Biểu đồ Độ ẩm theo thời gian

```
Type: Superchart / Timeline
├─ Virtual Pin: V1
├─ Chart Name: "Humidity Graph"
├─ Y-axis Label: "Humidity (%)"
├─ X-axis Label: "Time"
├─ Color: Blue
└─ Update Interval: 5 seconds
```

---

### 10. Hiển thị vị trí (Map)

```
Type: GPS/Map
├─ Virtual Pin: V7 (nếu có)
├─ Name: Device Location
└─ Display Mode: Map
```

---

### 11. Slider điều khỉnh Ngưỡng nhiệt độ

```
Type: Slider
├─ Virtual Pin: V8
├─ Name: "Temp Threshold"
├─ Min: 10
├─ Max: 40
├─ Step: 1
└─ Send value on release: checked
```

---

### 12. Menu thông tin thiết bị

```
Type: Menu
Options:
├─ Device Name: Group7-Monitor
├─ Firmware: v1.0.0
├─ Location: Lab Room 7
└─ Status: Online
```

---

## 🎨 Thiết kế giao diện tốt nhất

### Cách bố trí widgets:

```
┌──────────────┬──────────────┐
│  Temperature │   Humidity   │
│   Widget     │   Widget     │
├──────────────┴──────────────┤
│     Status Display           │
├──────────────┬──────────────┤
│  All LEDs    │   Green LED  │
├──────────────┼──────────────┤
│  Yellow LED  │   Red LED    │
├──────────────┴──────────────┤
│         Temperature Chart    │
├──────────────────────────────┤
│          Humidity Chart      │
└──────────────────────────────┘
```

---

## 🔔 Cấu hình Notifications

### Thêm Event Notification:

```
1. Settings → Events
2. Add Event:
   - Event: "Temp Alert"
   - Trigger: When V0 > 40
   - Action: Send notification
   - Message: "⚠️ Nhiệt độ quá cao!"
3. Save
```

---

## 🔐 Security Settings

### Bảo mật thiết bị:

```
1. Settings → Device Settings
2. Security:
   - Enable device verification
   - Set inactive timeout: 10 minutes
   - Enable two-factor auth (tùy chọn)
3. Save
```

---

## 📡 Kiểm tra kết nối

### Debug Blynk Connection:

```
Vào console (chữ nhỏ phía dưới):
- Connected: "Device connected"
- Error: "Device disconnected" atau "Auth token invalid"
- Data: Xem các virtual pin được update
```

---

## 🎯 Mẹo & Thủ thuật

### ⭐ Mẹo 1: Tạo Multiple Dashboards
```
- Dashboard 1: Monitoring (hiển thị dữ liệu)
- Dashboard 2: Control (điều khiển LED)
- Dashboard 3: Analytics (biểu đồ thống kê)
```

### ⭐ Mẹo 2: Sử dụng Templates
```
Blynk → Templates → Create Template
Định nghĩa lại các virtual pins để tái sử dụng
```

### ⭐ Mẹo 3: Automation (Rules)
```
Settings → Automations
Tạo kịch bản: Khi T > 35°C → Bật LED đỏ
```

### ⭐ Mẹo 4: Share Project
```
Ấn Share button → Invite friends
Friends có thể xem dữ liệu trong read-only mode
```

---

## ❌ Troubleshooting Blynk

### Lỗi: "Device Offline"
```
✓ Kiểm tra ESP8266 có kết nối WiFi không
✓ Kiểm tra token chính xác
✓ Khởi động lại ứng dụng Blynk
✓ Kiểm tra server status: https://status.blynk.io
```

### Lỗi: "Invalid Token"
```
✓ Sao chép token từ email lại
✓ Xóa token cũ, tạo token mới
✓ Kiểm tra không có khoảng trắng thừa
```

### Widget không cập nhật
```
✓ Kiểm tra Virtual Pin số chính xác
✓ Kiểm tra ESP8266 gửi dữ liệu (Serial Monitor)
✓ Thử F5 refresh app Blynk
```

### Nút bấm không phản ứng
```
✓ Kiểm tra BLYNK_WRITE handler trong code
✓ Kiểm tra Virtual Pin correct
✓ Khởi động lại ESP8266
```

---

## 📚 Tài liệu tham khảo

- [Blynk Docs](https://docs.blynk.io/)
- [Virtual Pins Guide](https://docs.blynk.io/en/concepts/virtual-pins)
- [Blynk Community](https://community.blynk.cc/)

---

**Phiên bản:** 1.0 | **Cập nhật:** March 2026 | **Nhóm:** 7
