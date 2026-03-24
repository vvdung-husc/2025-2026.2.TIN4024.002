# 🤖 Hướng dẫn cấu hình Telegram Bot

## 📲 Bước 1: Tạo Telegram Bot

### Tìm @BotFather

```
💬 Trên Telegram:

1. Mở app Telegram
2. Search: @BotFather (bot chính thức của Telegram)
3. Bấm nút "START"
```

---

## 🔧 Bước 2: Tạo Bot mới

### Conversation với @BotFather:

```
You: /newbot

BotFather: Alright! Let's create a new bot. 
          How are we going to call it? 
          Please choose a name for your bot.

You: Group7 Monitor

BotFather: Good. Now let's choose a username for your bot. 
          It must end in `bot`. For example, 
          TetrisBot or tetris_bot.

You: group7_monitor_bot

BotFather: Done! Congratulations on your new bot. 
           You will find it at t.me/group7_monitor_bot. 
           You can now add a description, about section 
           and profile picture for your bot, see /help 
           for a list of commands. By the way, when you've 
           finished creating your cool bot, ping our bot 
           developer at @BotSupport.

HERE IS YOUR TOKEN:
👉 123456:ABCDEf-qwerty-UIOPZXCVBNMASDFGH
```

**🎯 Sao chép Token này vào `#define BOTtoken`**

---

## 👤 Bước 3: Lấy Chat ID

### Tìm @userinfobot

```
💬 Trên Telegram:

1. Search: @userinfobot
2. Bấn "START"
3. Bot sẽ gửi thông tin của bạn:

   Id: 987654321
   is_bot: False
   first_name: Your
   last_name: Name
   username: yourname
   ...
```

**🎯 Sao chép ID (987654321) vào `#define CHAT_ID`**

---

## 🚀 Bước 4: Bắt đầu Bot

### Kích hoạt Bot

```
1. Search tên bot: @group7_monitor_bot
2. Bấn "START" hoặc gõ /start
3. Bot sẽ hiển thị menu (từ code Arduino của bạn)
4. Sẵn sàng nhận lệnh!
```

---

## 💬 Lệnh Telegram Bot có sẵn

### Available Commands

```
/start     - Xem menu trợ giúp
/temp      - Lấy nhiệt độ hiện tại
/humidity  - Lấy độ ẩm hiện tại
/status    - Xem trạng thái đầy đủ
/ledon     - Bật đèn LED
/ledoff    - Tắt đèn LED
```

### Ví dụ sử dụng:

```
👤 You:              🤖 Bot Response:
/start               Menu trợ giúp
/temp                Nhiệt độ: 25.5°C
/humidity            Độ ẩm: 60.3%
/status              Trạng thái: WARM
                     Nhiệt độ: 25.5°C
                     Độ ẩm: 60.3%
/ledon               Đèn LED đã bật
/ledoff              Đèn LED đã tắt
```

---

## 📋 Cấu hình Bot Commands

### Thiết lập lệnh mặc định

**Gửi lệnh cho @BotFather:**

```
BotFather, tôi muốn cấu hình lệnh cho bot

You: /setcommands

BotFather: Enter the list of commands as described here 
          (https://core.telegram.org/bots#creating-commands)

Format:
start - Menu trợ giúp
temp - Xem nhiệt độ
humidity - Xem độ ẩm
status - Xem trạng thái
ledon - Bật LED
ledoff - Tắt LED
```

---

## 🎨 Tùy chỉnh Bot Profile

### Cập nhật hình đại diện & mô tả

**Gửi lệnh cho @BotFather:**

```
You: /setabouttext

BotFather: Send me the new about text. 
          (0-120 characters)

You: Hệ thống giám sát nhiệt độ và độ ẩm của Nhóm 7

---

You: /setuserpic

BotFather: Send me a new profile picture 
          for your bot

(Gửi một ảnh)
```

---

## 📨 Gửi Cảnh báo Tự động

### Cảnh báo từ ESP8266 đến Telegram

Code sẵn trong `main.cpp`:

```cpp
void sendTelegramAlert(String message) {
  if (WiFi.status() != WL_CONNECTED) return;
  bot.sendMessage(CHAT_ID, message, "");
  Serial.println("Cảnh báo Telegram đã gửi: " + message);
}
```

### Kích hoạt cảnh báo:

```cpp
// Thêm vào loop() hoặc khi điều kiện thỏa mãn:

if (temperature > 40) {
  sendTelegramAlert("⚠️ CẢNH BÁO: Nhiệt độ quá cao!");
}

if (temperature < 5) {
  sendTelegramAlert("❄️ CẢNH BÁO: Nhiệt độ quá lạnh!");
}
```

---

## 🔐 Security & Best Practices

### 1. Bảo mật Token

```cpp
// ❌ KHÔNG bao giờ commit token lên GitHub
#define BOTtoken "YOUR_BOT_TOKEN"

// ✅ Thay vào file config.h riêng (gitignore)
// ✅ Sử dụng environment variables
```

### 2. Hạn chế truy cập

```cpp
// Kiểm tra Chat ID hợp lệ:
if (chat_id != CHAT_ID) {
  bot.sendMessage(chat_id, "Unauthorized", "");
  return;  // Không xử lý
}
```

### 3. Rate Limiting

```cpp
// Tránh spam:
if (lastAlertTime + ALERT_COOLDOWN > currentTime) {
  return;  // Bỏ qua nếu gửi quá gần
}
lastAlertTime = currentTime;
```

---

## 🐛 Troubleshooting

### Bot không phản ứng

```
✓ Kiểm tra token chính xác (không có khoảng trắng)
✓ Kiểm tra Chat ID chính xác
✓ Kiểm tra kết nối WiFi của ESP8266
✓ Xem Serial Monitor để debug
✓ Kiểm tra tường lửa không chặn Telegram
```

### Nhận được thông báo lỗi

```
❌ "Chat not found"
→ Chat ID không chính xác, lấy lại từ @userinfobot

❌ "Unauthorized"
→ Token bot không chính xác

❌ "API timeout"
→ Kết nối Internet có vấn đề

❌ "Too Many Requests"
→ Gửi tin nhắn quá nhanh, thêm delay
```

### Bot mất kết nối

```
✓ Kiểm tra loop() có chạy không
✓ Thêm try-catch để handle exceptions
✓ Kiểm tra buffer không overflow
✓ Giảm tần suất kiểm tra Telegram (từ 1s → 2s)
```

---

## 📊 Advanced Features

### 1. Inline Keyboard (Nút trong tin nhắn)

```cpp
String json = "";
json += "{\"inline_keyboard\":[[";
json += "{\"text\":\"🟢 Xanh\",\"callback_data\":\"led_green\"},";
json += "{\"text\":\"🟡 Vàng\",\"callback_data\":\"led_yellow\"},";
json += "{\"text\":\"🔴 Đỏ\",\"callback_data\":\"led_red\"}";
json += "]]}";

bot.sendMessageWithInlineKeyboard(
  CHAT_ID, 
  "Chọn đèn:", 
  json
);
```

### 2. Gửi ảnh/File

```cpp
// Gửi ảnh lên Telegram
bot.sendPhoto(CHAT_ID, "IMAGE_URL");

// Gửi file
bot.sendDocument(CHAT_ID, "FILE_URL");
```

### 3. Polling vs Webhooks

```
Current: Polling (kiểm tra thường xuyên)
- Đơn giản, không cần public IP
- Tiêu thụ bandwidth hơn

Alternative: Webhooks (Telegram gửi trực tiếp)
- Cần domain & SSL certificate
- Nhanh hơn
```

---

## 📱 Test Bot

### Danh sách kiểm tra:

```
[ ] Tạo được bot
[ ] Lấy được token
[ ] Lấy được chat ID
[ ] Bot phản ứng /start
[ ] Bot gửi /temp
[ ] Bot gửi /status
[ ] Nhận cảnh báo khi nhiệt độ cao
[ ] Bật LED từ Telegram
[ ] Tắt LED từ Telegram
```

---

## 📚 Tài liệu tham khảo

- [Telegram Bot API Docs](https://core.telegram.org/bots/api)
- [Telegram Bot Beginners Guide](https://core.telegram.org/bots)
- [UniversalTelegramBot Library](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)
- [Arduino JSON Documentation](https://arduinojson.org/)

---

## 💡 Mẹo & Trick

### 1. Auto-reply
```cpp
// Tự động trả lời khi nhận tin nhắn
if (msg.text == "hello") {
  bot.sendMessage(CHAT_ID, "Hello There! 👋", "");
}
```

### 2. Scheduled Messages
```cpp
// Gửi tin nhắn định kỳ
if (millis() - lastUpdate > 3600000) {  // Mỗi giờ
  bot.sendMessage(CHAT_ID, "Status: OK ✅", "");
  lastUpdate = millis();
}
```

### 3. Multi-language Support
```cpp
String getStatusMessage(String lang) {
  if (lang == "en") return "Status: OK";
  if (lang == "vi") return "Trạng thái: Tốt";
  return "Status: OK";
}
```

---

## 🎯 Production Checklist

- [ ] Token lưu trong biến riêng (không hardcode)
- [ ] Có error handling cho network failures
- [ ] Rate limiting để tránh spam
- [ ] Logging các message gửi/nhận
- [ ] Timeout cho các API calls
- [ ] Test lại toàn bộ lệnh trước deploy
- [ ] Backup token ở nơi an toàn

---

**Phiên bản:** 1.0 | **Cập nhật:** March 2026 | **Nhóm:** 7

Chúc bạn thành công! 🚀
