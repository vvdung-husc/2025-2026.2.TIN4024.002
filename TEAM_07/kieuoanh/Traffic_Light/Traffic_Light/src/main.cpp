#include <Arduino.h>
#include <TM1637Display.h>

// --- 1. KHAI BÁO CHÂN KẾT NỐI (Khớp với sơ đồ Wokwi) ---
#define LED_DO    15   // Đèn Đỏ
#define LED_VANG  2    // Đèn Vàng
#define LED_XANH  4    // Đèn Xanh
#define LED_DIBO  5    // Đèn xanh dương cho người đi bộ
#define BUTTON    18   // Nút bấm xin qua đường

// Chân màn hình LED 4 số (CLK và DIO)
#define CLK       22   
#define DIO       21   

// Khởi tạo thư viện màn hình
TM1637Display display(CLK, DIO);

// Biến lưu trạng thái nút bấm
bool coNguoiDiBo = false;

// --- 2. CẤU HÌNH BAN ĐẦU (SETUP) ---
void setup() {
  Serial.begin(115200);

  // Cấu hình chân đèn là OUTPUT (Xuất tín hiệu)
  pinMode(LED_DO, OUTPUT);
  pinMode(LED_VANG, OUTPUT);
  pinMode(LED_XANH, OUTPUT);
  pinMode(LED_DIBO, OUTPUT);

  // Cấu hình nút bấm là INPUT (Nhận tín hiệu)
  pinMode(BUTTON, INPUT_PULLUP);

  // Cài đặt độ sáng màn hình (0 là tối nhất, 7 là sáng nhất)
  display.setBrightness(5);
}

// --- HÀM PHỤ: ĐẾM NGƯỢC ---
// Hàm này hiển thị số đếm lùi và kiểm tra nút bấm cùng lúc
void demNguoc(int soGiay) {
  for (int i = soGiay; i >= 0; i--) {
    display.showNumberDec(i); // Hiện số lên màn hình
    
    // Kiểm tra nút bấm liên tục trong lúc chờ
    // Chia nhỏ 1 giây thành 10 lần kiểm tra để nút bấm nhạy hơn
    for (int j = 0; j < 10; j++) {
      if (digitalRead(BUTTON) == LOW) {
        coNguoiDiBo = true; // Ghi nhớ là có người bấm nút
        Serial.println("-> Đã nhận tín hiệu người đi bộ!");
      }
      delay(100); // Chờ 0.1 giây
    }
  }
}

// --- 3. VÒNG LẶP CHÍNH (LOOP) ---
void loop() {
  // === PHA 1: ĐÈN XANH (Xe được đi) ===
  Serial.println("Đèn Xanh bật");
  digitalWrite(LED_XANH, HIGH);
  digitalWrite(LED_VANG, LOW);
  digitalWrite(LED_DO, LOW);
  digitalWrite(LED_DIBO, LOW); // Tắt đèn đi bộ
  
  demNguoc(10); // Đếm lùi 10 giây
  
  // === PHA 2: ĐÈN VÀNG (Đi chậm) ===
  Serial.println("Đèn Vàng bật");
  digitalWrite(LED_XANH, LOW);
  digitalWrite(LED_VANG, HIGH);
  digitalWrite(LED_DO, LOW);
  
  demNguoc(3); // Đếm lùi 3 giây

  // === PHA 3: ĐÈN ĐỎ (Xe dừng - Người đi bộ được đi) ===
  Serial.println("Đèn Đỏ bật");
  digitalWrite(LED_XANH, LOW);
  digitalWrite(LED_VANG, LOW);
  digitalWrite(LED_DO, HIGH);

  // Nếu lúc nãy có người bấm nút thì bật đèn đi bộ
  if (coNguoiDiBo == true) {
     digitalWrite(LED_DIBO, HIGH); // Bật đèn xanh dương
     Serial.println("-> Mời người đi bộ qua đường");
  }

  demNguoc(10); // Đếm lùi 10 giây cho xe dừng

  // Kết thúc chu trình: Tắt đèn đi bộ và reset trạng thái nút bấm
  digitalWrite(LED_DIBO, LOW);
  coNguoiDiBo = false; 
}