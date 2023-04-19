// Source: https://randomnerdtutorials.com/esp32-i2c-communication-arduino-ide/
#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include "Adafruit_CAP1188.h"

// Configure LED pins as outputs. 
#define RED_PIN 33
#define YELLOW_PIN 32
#define GREEN_PIN 39
#define WHITE_PIN 38

#define BUZZER_PIN 17
int ledState = LOW;
bool ledFlash = false;

int count = 0; 

TFT_eSPI tft = TFT_eSPI(); 


Adafruit_CAP1188 cap = Adafruit_CAP1188();

void setup() {
  tft.init();
  tft.setRotation(1);
  tft.width();
  tft.height();
  tft.setTextSize(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);

  pinMode(RED_PIN, OUTPUT); 
  pinMode(YELLOW_PIN, OUTPUT); 
  pinMode(GREEN_PIN, OUTPUT);

  Wire.begin();
  Serial.begin(9600);

  Serial.println("CAP1188 test!");
  if (!cap.begin()) {
    Serial.println("CAP1188 not found");
    while (1);
  }
  Serial.println("CAP1188 found!");
}

 
void loop() {
    // OLED display when the button sensor is pressed
    uint8_t touched = cap.touched();
    if (touched != 0) {
        Serial.println("Botton pressed");
        count = 0;
        ledFlash = true;
        tft.drawString("Bell's ringing", 10, 10, 2);
    }

    // Control led lights and buzzer
    if (ledFlash == true)
    {
      if (ledState == HIGH) {ledState = LOW;}
      else
      {
        tone(BUZZER_PIN, 250, 200);
        ledState = HIGH;
      }
    }
    digitalWrite(RED_PIN, ledState); 
    digitalWrite(YELLOW_PIN, ledState); 
    digitalWrite(GREEN_PIN, ledState);
    
    // Turn off all alerts 5s after the button is pressed
    count += 200;
    if (count >= 5000){
      count = 0;
      tft.fillScreen(TFT_BLACK);
      ledState = LOW;
      ledFlash = false;
    }
    delay(200);
}