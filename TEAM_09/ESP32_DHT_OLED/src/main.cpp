/*
THÔNG TIN NHÓM 9
1. Nguyễn Đăng Khôi
2. Nguyễn Gia Huy
3. Nguyễn Lê Bá Minh
4. Nguyễn Xuân Minh Hiếu
5. Nguyễn Nhật Hào
*/

#include "main.h"

void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  dht.begin();
  setupOLED();
}

void loop() {
  static bool firstRun = true;     // chỉ in lần đầu
  static String lastStatus = "";   // lưu trạng thái trước đó

  float temp = dht.readTemperature();
  float humi = dht.readHumidity();

  if (isnan(temp) || isnan(humi)) {
    Serial.println("DHT error");
    return;
  }

  String status;

  if (temp < 13) status = "TOO COLD";
  else if (temp < 20) status = "COLD";
  else if (temp < 25) status = "COOL";
  else if (temp < 30) status = "WARM";
  else if (temp < 35) status = "HOT";
  else status = "TOO HOT";

  /* ===== SERIAL MONITOR (ONLY WHEN CHANGE OR FIRST RUN) ===== */
  if (firstRun || status != lastStatus) {
    Serial.print("Temp: ");
    Serial.print(temp, 2);
    Serial.print(" °C | Humi: ");
    Serial.print(humi, 2);
    Serial.print(" % | Status: ");
    Serial.print(status);
    Serial.print(" | LED: ");

    if (status == "TOO COLD" || status == "COLD") {
      Serial.println("GREEN");
    }
    else if (status == "COOL" || status == "WARM") {
      Serial.println("YELLOW");
    }
    else {
      Serial.println("RED");
    }

    lastStatus = status;
    firstRun = false;
  }

  updateOLED(temp, humi, status);
  updateLED(temp);

  delay(200);
}
