#include <Arduino.h>
#include <DHT.h>

// ======= CẤU HÌNH =======
#define LED_PIN LED_BUILTIN
#define DHT_PIN 4          // GPIO4 (D2 trên NodeMCU)

// Chọn đúng loại DHT theo module bạn đang dùng
// - DHT11: giá trị nhỏ, ổn định, tần số đọc thấp (tối đa 1 lần/2s)
// - DHT22: chính xác hơn, hoạt động tốt trong dải rộng hơn
#define DHT_TYPE DHT11     // Thay thành DHT22 nếu bạn dùng DHT22

// ========================

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  delay(100); // Đợi Serial ổn định

  pinMode(LED_PIN, OUTPUT);
  dht.begin();

  // Đợi sensor khởi động
  delay(2000);

  Serial.println("--- ESP8266 DHT Test ---");
  Serial.print("DHT pin: ");
  Serial.println(DHT_PIN);
  Serial.print("DHT type: ");
  Serial.println((DHT_TYPE == DHT11) ? "DHT11" : "DHT22");
  Serial.println("-----------------------");
}

void loop() {
  // Đọc nhiệt độ và độ ẩm
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Kiểm tra nếu đọc được giá trị
  if (isnan(h) || isnan(t)) {
    Serial.println("Lỗi đọc từ DHT! Kiểm tra kết nối.");
  } else {
    Serial.print("Nhiệt độ: ");
    Serial.print(t);
    Serial.print("°C, Độ ẩm: ");
    Serial.print(h);
    Serial.println("%");
  }
  
  // Bật đèn LED (Lưu ý: LOW là Sáng đối với đa số board ESP8266)
  digitalWrite(LED_PIN, LOW);
  delay(1000); // Đợi 1 giây

  // Tắt đèn LED (HIGH là Tắt)
  digitalWrite(LED_PIN, HIGH);
  delay(5000); // Đợi 5 giây
}