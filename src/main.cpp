#include <Arduino.h>
#include <Wire.h>

#include <LiquidCrystal.h>
#include <Thread.h>
#include <ThreadController.h>

// Constants --
#define LCD_UPDATE_TIME_MS 250
#define LCD_W 16
#define LCD_H 2

#define O2_SENSOR_PIN A0

#define AFR_SLOPE 2
#define AFR_INTERCEPT 10
#define LAMBDA_SLOPE 0.136
#define LAMBDA_INTERCEPT 0.68

enum OutputType { AFRType, LambdaType };

// Globals --
ThreadController controller;
Thread lcdThread;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

float afr = 0.0;
float lambda = 0.0;

void initlcd() {
  lcd.begin(LCD_W, LCD_H);
  lcd.home();
  lcd.print(F("AFR:"));
  lcd.setCursor(0, 1);
  lcd.print(F("Lambda:"));
}

void updatelcd() {
  lcd.setCursor(9, 0);
  lcd.print(F("      "));
  lcd.setCursor(9, 0);
  lcd.print(afr);

  lcd.setCursor(9, 1);
  lcd.print(F("      "));
  lcd.setCursor(9, 1);
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

void setup() {
  Serial.begin(115200);
  initlcd();

  lcdThread.onRun(updatelcd);
  lcdThread.setInterval(LCD_UPDATE_TIME_MS);
  controller.add(&lcdThread);
}

void loop() {
  controller.run();

  int reading = analogRead(O2_SENSOR_PIN);
  float voltage = reading * (5.0 / 1023.0);
  afr = convertOutput(voltage, OutputType::AFRType);
  lambda = convertOutput(voltage, OutputType::LambdaType);
}
