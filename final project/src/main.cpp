#include <OneButton.h> //button
#include <Adafruit_NeoPixel.h> //led strip
#include <colors.h> //text color
#include <IoTTimer.h> //callback to client structure
#include <Wire.h>
#include <Adafruit_GFX.h> //graphic display
#include <Adafruit_SSD1306.h> //monochrome oleds
#include <SPI.h>
#include <Ethernet.h> //connect to internet throught ethernet shield
#include <mac.h> //apple mac
#include <hue.h> //led light hues
#include <wemo.h> //wemo device wifi

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define SCREEN_ADDRESS 0X3C
#define pirPin 1
#define ledPin1 4

int PIXELPIN = 5;
int PIXELCOUNT = 128;
int val = 0;
const int BUTTONPIN = 23;
bool buttonState;
bool motionState = false;
bool displayShown = false;
bool turnThingsOff;
int pixels;
unsigned int frequency = 396;
unsigned long duration = 1000;
int currentTime;
int lastTime;
int color;

OneButton button1(BUTTONPIN, false); //define objects
Adafruit_NeoPixel pixel(PIXELCOUNT, PIXELPIN, NEO_GRB + NEO_KHZ800 );
IoTTimer timer;
Adafruit_SSD1306 display (SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//EthernetClient client;
bool status;   //user to ensure port openned correctly
byte thisbyte; //used to get IP address

void setup() {
  Serial.begin (9600) ; // begin processes
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  button1.attachClick(click); // initialize objects

  //initialize ethernet port and uSD module to off
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  //Open Serial Communication and wait for port to open:
  Serial.begin(9600);
  while (!Serial);

  Serial.printf("Starting Program:\n");

  //    Start ethernet connection
  status = Ethernet.begin(mac);
  if (!status) {
    Serial.printf("failed to configure Ethernet using DHCP \n");
    //no point in continueing
    while (1);
  }

  //print your local IP address
  Serial.print("My IP address: ");
  for (thisbyte = 0; thisbyte < 3; thisbyte++) {
    //print value of each byte of the IP address
    Serial.printf("%i.", Ethernet.localIP()[thisbyte]);
  }
  Serial.printf("%i\n", Ethernet.localIP()[thisbyte]);
  display.display();
  delay(2000);
  pixel.begin();
  pixel.show();
  pixel.setBrightness(25);
  button1.attachClick(click); // initialize objects
  buttonState = false ; // set variables

  pinMode(ledPin, OUTPUT);
  pinMode (pirPin, INPUT);
}

void loop() {
  button1.tick();
  currentTime = millis();
  if (buttonState) {
    Serial.printf("button state %i\n", buttonState);
    doorbellringingtext();
    tone(2, frequency, duration);
    switchON(0);
    switchON(2);
    switchON(3);
    for (int i = 1; i <= 6; i++) {
      setHue(i, true, HueRainbow[color], 100, 255);
      color++;
      if (color > 7) {
        color = 0;
      }
    }
    for (int i = 0; i < 2; i++) {
      pixel.fill(blue, 0, PIXELCOUNT);
      pixel.show();
      delay(1000);
      pixel.clear();
      pixel.show();
      delay(1000);
    }
    pixel.clear();
    pixel.show();
    pixel.fill(blue, 0, PIXELCOUNT);
    pixel.show();
    delay(4000);
    buttonState = false;
    turnThingsOff = true;
  }
  //this else turns everything off
  else {
    if (turnThingsOff) {
      pixel.clear();
      pixel.show();
      noTone(2);
      for (int i = 1 ; i <= 6; i++) {
        setHue(i, false, 0, 0, 0);
      }
      switchOFF(0);
      switchOFF(2);
      switchOFF(3);
      turnThingsOff = false;
    }
  }
  motionState = digitalRead(pirPin);
  if (motionState == true) {
    Serial.println("Motion detected!");
    digitalWrite(ledPin, HIGH);
    if (!displayShown) {
      motiondetected();
      displayShown = true;
      timer.startTimer(1000);
    }
  }
  else {
    if (timer.isTimerReady()) {
      displayShown = false;
      display.clearDisplay();
      display.display();
      digitalWrite(ledPin, LOW);
    }
  }
}

void click () {
  buttonState = true;
  Serial.printf("button state %i\n", buttonState);
}

//Text that displays when someone rings doorbell
void doorbellringingtext (void) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setRotation(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.printf("Doorbell\n Ringing!");
  display.display();
}
// Text that displays when motion is detected
void motiondetected (void) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setRotation(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.printf("Motion\n detected!");
  display.display();
}