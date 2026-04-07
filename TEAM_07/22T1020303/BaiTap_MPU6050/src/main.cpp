#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
int ledPin = 7;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);   // Bật LED đỏ ngay từ đầu

  Wire.begin();
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU6050 Connected Successfully!");
  } else {
    Serial.println("MPU6050 Connection Failed!");
  }
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // In thông số ra Serial Monitor
  Serial.print("AX: "); Serial.print(ax);
  Serial.print("  AY: "); Serial.print(ay);
  Serial.print("  AZ: "); Serial.println(az);

  Serial.print("GX: "); Serial.print(gx);
  Serial.print("  GY: "); Serial.print(gy);
  Serial.print("  GZ: "); Serial.println(gz);

  // Tính giá trị Acceleration (g) và Rotation (°/sec) đơn giản
  float accelX = ax / 16384.0;
  float accelY = ay / 16384.0;
  float accelZ = az / 16384.0;

  Serial.print("Acceleration (g) -> X: "); Serial.print(accelX, 2);
  Serial.print("  Y: "); Serial.print(accelY, 2);
  Serial.print("  Z: "); Serial.println(accelZ, 2);

  Serial.println("-------------------------");

  delay(500);   // Cập nhật mỗi 0.5 giây
}