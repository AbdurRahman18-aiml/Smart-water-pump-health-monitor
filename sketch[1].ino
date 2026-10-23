
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

// Pin connections - keep your existing circuit
#define FAULT_BUTTON 14
#define GREEN_LED 25
#define RED_LED 26
#define BUZZER_PIN 27

Adafruit_MPU6050 mpu;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Vibration thresholds
const float WARNING_RMS = 0.15;
const float CRITICAL_RMS = 0.30;
const float VIBRATION_BASELINE = 9.80665;

// Simulated water flow
float simulatedFlow = 5.0;  // Liters per minute
float flowLMin = 5.0;
float totalLiters = 0.0;

// Vibration sampling
const int RMS_SAMPLES = 100;
float accelSquaredSum = 0;
int rmsSampleCount = 0;
float vibrationRMS = 0;

// Automatic vibration test:
// 0 = LOW, 1 = WARNING, 2 = HIGH
int vibrationTestMode = 0;
unsigned long lastVibrationTestChange = 0;
const unsigned long TEST_INTERVAL = 5000;

// Timing
unsigned long lastMeasure = 0;
unsigned long lastLCD = 0;
unsigned long lastFlowUpdate = 0;
unsigned long lastScreenChange = 0;

bool sensorOK = false;
bool faultActive = false;
int lcdScreen = 0;

void setup() {
  pinMode(FAULT_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Pump Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  sensorOK = mpu.begin();

  if (sensorOK) {
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }

  delay(1200);
  lcd.clear();

  unsigned long now = millis();
  lastVibrationTestChange = now;
  lastFlowUpdate = now;
  lastScreenChange = now;
}

void loop() {
  unsigned long now = millis();

  // Cycle vibration test modes every five seconds
  if (now - lastVibrationTestChange >= TEST_INTERVAL) {
    lastVibrationTestChange = now;
    vibrationTestMode = (vibrationTestMode + 1) % 3;
  }

  // Read MPU6050 every 10 milliseconds
  if (now - lastMeasure >= 10) {
    lastMeasure = now;

    if (sensorOK) {
      sensors_event_t accel, gyro, temp;
      mpu.getEvent(&accel, &gyro, &temp);

      float ax = accel.acceleration.x;
      float ay = accel.acceleration.y;
      float az = accel.acceleration.z;

      float magnitude = sqrt(
        ax * ax + ay * ay + az * az
      );

      float deviation = magnitude - VIBRATION_BASELINE;

      accelSquaredSum += deviation * deviation;
      rmsSampleCount++;

      if (rmsSampleCount >= RMS_SAMPLES) {
        vibrationRMS = sqrt(
          accelSquaredSum / RMS_SAMPLES
        );

        accelSquaredSum = 0;
        rmsSampleCount = 0;
      }

      // Test values for checking the display and alarm
      if (vibrationTestMode == 0) {
        vibrationRMS = 0.05;
      } else if (vibrationTestMode == 1) {
        vibrationRMS = 0.20;
      } else {
        vibrationRMS = 0.40;
      }
    }

    faultActive = (digitalRead(FAULT_BUTTON) == LOW);
  }

  // Simulate a constant water flow and accumulate volume
  unsigned long elapsed = now - lastFlowUpdate;

  if (elapsed > 0) {
    lastFlowUpdate = now;

    flowLMin = simulatedFlow;

    // Liters = L/min multiplied by elapsed minutes
    totalLiters += simulatedFlow * elapsed / 60000.0;
  }

  bool critical = faultActive ||
                  !sensorOK ||
                  vibrationRMS >= CRITICAL_RMS;

  bool warning = vibrationRMS >= WARNING_RMS;

  // LEDs
  digitalWrite(
    GREEN_LED,
    (!critical && !warning) ? HIGH : LOW
  );

  digitalWrite(
    RED_LED,
    (critical || warning) ? HIGH : LOW
  );

  // Buzzer
  if (critical) {
    tone(BUZZER_PIN, 2000);
  } else if (warning) {
    tone(BUZZER_PIN, 1000);
  } else {
    noTone(BUZZER_PIN);
  }

  // Alternate LCD screens every 2.5 seconds
  if (now - lastScreenChange >= 2500) {
    lastScreenChange = now;
    lcdScreen = 1 - lcdScreen;
    lcd.clear();
  }

  // Refresh LCD
  if (now - lastLCD >= 250) {
    lastLCD = now;

    if (lcdScreen == 0) {
      // Screen 1: vibration status
      lcd.setCursor(0, 0);

      if (!sensorOK) {
        lcd.print("MPU6050 ERROR   ");
      } else if (faultActive) {
        lcd.print("PUMP FAULT      ");
      } else if (vibrationRMS >= CRITICAL_RMS) {
        lcd.print("VIB: HIGH       ");
      } else if (warning) {
        lcd.print("VIB: WARNING    ");
      } else {
        lcd.print("VIB: LOW        ");
      }

      lcd.setCursor(0, 1);
      lcd.print("RMS:");
      lcd.print(vibrationRMS, 2);
      lcd.print(" m/s2  ");

    } else {
      // Screen 2: water flow
      lcd.setCursor(0, 0);
      lcd.print("Flow: ");
      lcd.print(flowLMin, 1);
      lcd.print(" L/min  ");

      lcd.setCursor(0, 1);
      lcd.print("Total: ");
      lcd.print(totalLiters, 2);
      lcd.print(" L     ");
    }
  }

  delay(2);
}








