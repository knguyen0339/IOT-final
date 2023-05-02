// Source: https://randomnerdtutorials.com/esp32-i2c-communication-arduino-ide/
#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include "Adafruit_CAP1188.h"
#include <HttpClient.h>
#include <WiFi.h>

// Configure pins 
#define RED_PIN 33
#define YELLOW_PIN 25
#define GREEN_PIN 26
#define WHITE_PIN 27

#define PIR_PIN 38
#define BUZZER_PIN 17

// Camera pins
#define CS_PIN 2
#define MOSI_PIN 15
#define MISO_PIN 13
#define SCK_PIN 12
#define OV2640_CHIPID_HIGH 	0x0A
#define OV2640_CHIPID_LOW 	0x0B

int ledState = LOW;
bool pressed = false;
bool pirTriggered = false;

int ledCount = 0; //Track the time after the button is pressed
int pirCount = 0; //Track the time after the pir sensor is triggered
int var;

time_t current_time;

TFT_eSPI tft = TFT_eSPI(); 

Adafruit_CAP1188 cap = Adafruit_CAP1188();

// Network credentials
char ssid[] = "OnlyAn";    // your network SSID (name) 
char pass[] = "C@lSt@te"; // your network password (use for WPA, or use as key for WEP)
const char kHostname[] = "3.136.233.192";
const char kPath[] = "/?"; 
// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;


void cloud_connect(int var){
  int err =0;
  String newPath = String(kPath) + "var=" + String(var);
  const char *kPath1 = newPath.c_str();

  
  WiFiClient c;
  HttpClient http(c);
  
  err = http.get(kHostname, 5000, kPath1);
  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character
                Serial.print(c);
               
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
}

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
  
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

 
void loop() {
    var = 0;

    int pirValue = digitalRead(PIR_PIN); // Read the PIR sensor value
    if (pirValue == HIGH && pirTriggered == false && pressed == false) { // If the PIR sensor detects motion
        Serial.println("Motion detected"); 
        var = 1;
        cloud_connect(var);
        pirCount = 0;
        pirTriggered = true;
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Motion detected", 10, 10, 2);
      } 

    // OLED display when the button sensor is pressed
    uint8_t touched = cap.touched();
    if (touched != 0 && pressed == false) {
        Serial.println("Botton pressed");
        var = 2;
        cloud_connect(var);
        ledCount = 0;
        pressed = true;
        tone(BUZZER_PIN, 250, 2000);
        tft.fillScreen(TFT_BLACK);
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
    if (ledCount >= 10000){
      ledCount = 0;
      tft.fillScreen(TFT_BLACK);
      ledState = LOW;
      pressed = false;
    }

    //Pause pir sensor for 10 seconds after triggered
    pirCount += 200;
    if(pirCount >= 10000){
      pirCount = 0;
      pirTriggered = false;
    }

    

    delay(200);
}