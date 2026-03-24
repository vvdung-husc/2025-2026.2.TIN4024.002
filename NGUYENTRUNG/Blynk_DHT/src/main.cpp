#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6Y_0RtyKt"
#define BLYNK_TEMPLATE_NAME "BlynkDHT"
#define BLYNK_AUTH_TOKEN "vHtJcBiqxcCu8u7xsYnIrcinqMzuANm1"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TM1637Display.h>
#include <DHT.h>

/* WIFI */
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

/* PIN CONFIG */
#define btnBLED 23
#define pinBLED 21

#define CLK 18
#define DIO 19

#define DHTPIN 16
#define DHTTYPE DHT22

/* OBJECT */
TM1637Display display(CLK, DIO);
DHT dht(DHTPIN, DHTTYPE);

/* VARIABLE */
bool blueButtonON = false;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0; // Biến dùng cho thuật toán millis()
unsigned long operatingTime = 0; // Biến đếm thời gian hoạt động

/* TIMER */
BlynkTimer timer;

/* BLYNK CONNECTED EVENT */
// Hàm này sẽ tự động chạy ngay khi ESP32 kết nối thành công với Server Blynk
BLYNK_CONNECTED() {
  Serial.println("Đã kết nối với Blynk Server!");
  
  // Gửi tên lên datastream V5
  Blynk.virtualWrite(V5, "Nguyen Trung");
  
  // Đồng bộ tất cả trạng thái từ Web/App về phần cứng
  Blynk.syncAll(); 
}

/* BUTTON FUNCTION */
void checkButton() {
  int state = digitalRead(btnBLED);

  if (state == LOW && lastButtonState == HIGH) {
    // Sử dụng millis() thay cho delay(200) để không làm chặn Blynk.run()
    if ((millis() - lastDebounceTime) > 200) {
      blueButtonON = !blueButtonON;
      digitalWrite(pinBLED, blueButtonON);

      // đồng bộ với Blynk
      Blynk.virtualWrite(V1, blueButtonON);

      if (!blueButtonON) display.clear();
      
      lastDebounceTime = millis(); // Cập nhật lại thời gian
    }
  }
  lastButtonState = state;
}

/* TIME FUNCTION */
void sendTime() {
  // Chỉ cộng thời gian và gửi đi nếu nút đang BẬT
  if (blueButtonON) {
    operatingTime++; // Mỗi giây tăng thêm 1
    
    // Gửi lên datastream Time (V0) trên Blynk
    Blynk.virtualWrite(V0, operatingTime);

    // Hiển thị lên LED 7 đoạn
    display.showNumberDec(operatingTime);
  }
}

/* READ DHT22 */
void readDHT() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(temp) && !isnan(hum)) {
    // gửi đúng datastream
    Blynk.virtualWrite(V2, temp);
    Blynk.virtualWrite(V3, hum);

    Serial.print("Temp: ");
    Serial.println(temp);

    Serial.print("Hum: ");
    Serial.println(hum);
  }
}

/* BLYNK BUTTON */
BLYNK_WRITE(V1) {
  blueButtonON = param.asInt();
  digitalWrite(pinBLED, blueButtonON);

  if (!blueButtonON) display.clear();
}

/* SETUP */
/* SETUP */
void setup() {
  Serial.begin(115200);

  pinMode(pinBLED, OUTPUT);
  pinMode(btnBLED, INPUT_PULLUP);

  display.setBrightness(7);
  dht.begin();

  // 1. KẾT NỐI WIFI (TỐI ƯU CHO WOKWI)
  Serial.print("Đang kết nối WiFi...");
  
  // Wokwi-GUEST hoạt động ổn định nhất ở Channel 6
  WiFi.begin(ssid, pass, 6); 
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi đã kết nối thành công!");

  // QUAN TRỌNG: Chờ hệ thống Wokwi cấp phát xong DNS Server
  delay(2000); 

  Serial.print("Địa chỉ IP mạng ảo: ");
  Serial.println(WiFi.localIP());

  // 2. CẤU HÌNH VÀ KẾT NỐI BLYNK
  Serial.println("Đang phân giải DNS và kết nối Blynk...");
  Blynk.config(BLYNK_AUTH_TOKEN, IPAddress(128, 199, 144, 129), 80);
  Blynk.connect();

  timer.setInterval(1000L, sendTime);
  timer.setInterval(2000L, readDHT);
}

/* LOOP */
void loop() {
  Blynk.run();
  timer.run();
  checkButton();
}