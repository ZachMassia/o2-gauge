#include <Arduino.h>
#include <Wire.h>

#include <LiquidCrystal.h>
#include <Thread.h>
#include <ThreadController.h>


// Constants --
#define LCD_UPDATE_TIME_MS 250
#define LCD_W 16
#define LCD_H 2

// Globals --
ThreadController controller;
Thread lcdThread;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

float AFR = 0.0;
float lambda = 0.0;

// Function forward declarations --
void initLCD();
void updateLCD();


void setup() { // -------------------------------------------------------------
    Serial.begin(115200);
    initLCD();
    
    lcdThread.onRun(updateLCD);
    lcdThread.setInterval(LCD_UPDATE_TIME_MS);
    controller.add(&lcdThread);
}

void loop() { // --------------------------------------------------------------
    controller.run();
}


// Functions --

void initLCD() {
    lcd.begin(LCD_W, LCD_H);
    lcd.home();
    lcd.print(F("AFR:"));
    lcd.setCursor(0, 1);
    lcd.print(F("Lambda:"));
}

void updateLCD() {
    lcd.setCursor(9, 0);
    lcd.print(F("      "));
    lcd.setCursor(9, 0);
    lcd.print(AFR);
    
    lcd.setCursor(9, 1);
    lcd.print(F("      "));
    lcd.setCursor(9, 1);
    lcd.print(lambda);
}
