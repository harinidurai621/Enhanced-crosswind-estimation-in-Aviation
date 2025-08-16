#include <Wire.h>
#include <SoftwareWire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050.h>
// OLED display setup (uses hardware I2C A4 = SDA, A5 = SCL)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// Software I2C for MPU6050 on A1 (SDA), A2 (SCL)
SoftwareWire swWire(A1, A2);     // SDA, SCL
MPU6050 mpu(&swWire);            // Pass SoftwareWire pointer to MPU
// Low-pass filter constant
const float alpha = 0.1;
float filteredX = 0;
void setup() {
  Serial.begin(9600);
  // OLED initialization
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Initializing MPU...");
  display.display();
  // MPU initialization
  swWire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 init failed!");
    display.setCursor(0, 10);
    display.println("MPU init failed");
    display.display();
    while (true);
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("MPU6050 Ready!");
  display.display();
  delay(1000);
}
void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  // Convert raw acceleration to 'g' units (assuming ±2g range)
  float rawX = (float)ax / 16384.0;
  // Apply simple low-pass filter
  filteredX = alpha * rawX + (1 - alpha) * filteredX;
  // Estimate wind speed (tuned factor – adjust as needed)
  float windSpeed = abs(filteredX) * 70.0;  // Approximation
  // Display on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Crosswind Estimator");
  display.setCursor(0, 16);
  display.print("Accel X: ");
  display.print(rawX, 3);
  display.println(" g");
  display.setCursor(0, 32);
  display.print("Filtered: ");
  display.print(filteredX, 3);
  display.println(" g");
  display.setCursor(0, 48);
  display.print("Est. Wind: ");
  display.print(windSpeed, 1);
  display.println(" m/s");
  display.display();
  delay(200);
}

