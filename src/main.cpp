#include <Arduino.h>
#include <Wire.h>

#include <TKLCD.h>
#include <Thread.h>
#include <ThreadController.h>

// Constants --
#define STARTUP_MSG_TIME_MS 1325
#define LCD_UPDATE_TIME_MS 250

#define O2_SENSOR_PIN A0

#define AFR_SLOPE 2
#define AFR_INTERCEPT 10
#define LAMBDA_SLOPE 0.136
#define LAMBDA_INTERCEPT 0.68

enum OutputType { AFRType, LambdaType };

// Globals --
ThreadController controller;
Thread lcdThread;
TKLCD_Local lcd = TKLCD_Local();

float voltage = 0.0;
float afr = 0.0;
float lambda = 0.0;

// ----------------------------------------------------------------------------

void initlcd() {
  // Configure the LCD
  lcd.begin();
  lcd.clear();

  // Display startup message
  lcd.print(F("    O2 GAUGE    "));
  lcd.setCursor(0, 1);
  lcd.print(F("   BOOTING...   "));
  delay(STARTUP_MSG_TIME_MS);
  lcd.clear();

  // Print one-time headers
  lcd.print(F("AFR"));
  lcd.setCursor(0, 1);
  lcd.print(F("Lambda"));
  lcd.setCursor(15, 0);
  lcd.print(F("V"));
}

void updatelcd() {
  // Update AFR
  lcd.setCursor(4, 0);
  lcd.print(afr);

  // Update Voltage
  lcd.setCursor(11, 0);
  lcd.print(voltage);

  // Update lambda
  lcd.setCursor(7, 1);
  lcd.print(lambda);
}

float convertOutput(float val, OutputType t) {
  switch (t) {
  case OutputType::AFRType:
    return (val * AFR_SLOPE) + AFR_INTERCEPT;
  case OutputType::LambdaType:
    return (val * LAMBDA_SLOPE) + LAMBDA_INTERCEPT;
  default:
    return 0.0;
  }
}

// ----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  initlcd();

  lcdThread.onRun(updatelcd);
  lcdThread.setInterval(LCD_UPDATE_TIME_MS);
  controller.add(&lcdThread);
}

// ----------------------------------------------------------------------------
void loop() {
  controller.run();

  voltage = analogRead(O2_SENSOR_PIN) * (5.0 / 1023.0);
  afr = convertOutput(voltage, OutputType::AFRType);
  lambda = convertOutput(voltage, OutputType::LambdaType);
}
