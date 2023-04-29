// Source: https://randomnerdtutorials.com/esp32-i2c-communication-arduino-ide/
#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include "Adafruit_CAP1188.h"

// Configure pins 
#define RED_PIN 33
#define YELLOW_PIN 25
#define GREEN_PIN 26
#define WHITE_PIN 27

#define PIR_PIN 38
#define BUZZER_PIN 17

int ledState = LOW;
bool pressed = false;
bool pirTriggered = false;

int ledCount = 0; //Track the time after the button is pressed
int pirCount = 0; //Track the time after the pir sensor is triggered

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
  pinMode(WHITE_PIN, OUTPUT);

  Wire.begin();
  Serial.begin(9600);

  pinMode(PIR_PIN, INPUT); // Set the PIR sensor pin as an input

  Serial.println("CAP1188 test!");
  if (!cap.begin()) {
    Serial.println("CAP1188 not found");
    while (1);
  }
  Serial.println("CAP1188 found!");
}

 
void loop() {
    int pirValue = digitalRead(PIR_PIN); // Read the PIR sensor value
    if (pirValue == HIGH && pirTriggered == false) { // If the PIR sensor detects motion
        Serial.println("Motion detected"); 
        pirCount = 0;
        pirTriggered = true;

        // Take a picture
        
      } 

    // OLED display when the button sensor is pressed
    uint8_t touched = cap.touched();
    if (touched != 0) {
        Serial.println("Botton pressed");
        ledCount = 0;
        pressed = true;
        tone(BUZZER_PIN, 250, 2000);
        tft.drawString("Bell's ringing", 10, 10, 2);
    }

    // Control led lights and buzzer
    if (pressed == true)
    {
      if (ledState == HIGH) {ledState = LOW;}
      else
      {
        ledState = HIGH;
      }
    }
    digitalWrite(RED_PIN, ledState); 
    digitalWrite(YELLOW_PIN, ledState); 
    digitalWrite(GREEN_PIN, ledState);
    digitalWrite(WHITE_PIN, ledState);
    
    // Turn off all alerts 5s after the button is pressed
    ledCount += 200;
    if (ledCount >= 5000){
      ledCount = 0;
      tft.fillScreen(TFT_BLACK);
      ledState = LOW;
      pressed = false;
    }

    //Pause pir sensor for 10 seconds after triggered
    pirCount += 200;
    if(pirCount >= 1000){
      pirCount = 0;
      pirTriggered = false;
    }

    delay(200);

}