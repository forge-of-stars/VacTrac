/*  Orion Rozance
 *  Last modified: December 2 2022
 *  Integrated code
 *  LCD: 240x320 res
 */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <avr/interrupt.h>
#include <avr/io.h>

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Color Definitions
#define BLACK 0x0083
#define WHITE 0xefbb
#define RED 0xfaea
#define PURPLE 0xbd9d
#define BLUE 0x22af

// more pin definitions
#define photoPin 23

// global variables
volatile boolean intFlag = 0;
volatile int rectX;    // keep track of x coordinate of rectangles. will increment with time
volatile int rectY;    // keep track of y coordinate of rectangles. will increment with bubble events
int w;
int hInst;    // height for instant events
int hCont;    // height for continuous events
volatile unsigned long eventStart;   // at start of ISR/start of bubble event
volatile unsigned long eventEnd = 0;     // end of bubble event
volatile unsigned long eventDuration;    // duration of bubble event

volatile unsigned long tme;
volatile unsigned long sec;
volatile unsigned long mins;
volatile unsigned long hr;
unsigned long elapsedTime;      // time since patient was last checked
int value;


void setup() {
  // put your setup code here, to run once:
  tft.begin();
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2);
  tft.setTextWrap(1);
  
  attachInterrupt(digitalPinToInterrupt(photoPin), measureDraw, FALLING);
  
  w = 20;     // width of rectangles in pixels
  rectX = 20;   // x for initial rectangle
  rectY = 240;    // y for initial rectangle (at bottom of screen)
  hInst = -100;    // height of rectangle in pixels. negative because of coordinate frame of screen
  hCont = -120;

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  value = digitalRead(photoPin);
  Serial.println(value);
  delay(1000);

  // display time since patient last checked/reset
  elapsedTime = millis();
  int etMins = elapsedTime/60000;    // elapsed minutes
  int etHr = etMins/60;              // elapsed hours
  
  tft.setCursor(0, 0);
  tft.print("LAST CHECKED: ");
  if(etHr < 10) {
    tft.print("0");
  }
  tft.print(etHr); tft.print(":");
  if(etMins < 10) {
    tft.print("0");
  }
  tft.print(etMins);

  // display time since last bubble
  while(intFlag == 1) {
    tme = millis()-eventEnd;
    sec = tme/1000;
    mins = sec/60;
    hr = mins/60;
    tme %= 1000;
    sec %= 60;
    mins %= 60;
    hr %= 24;

    tft.setCursor(0, 10);
    tft.print("LAST BUBBLE: ");
    if(hr < 10) {
      tft.print("0");
    }
    tft.print(hr); tft.print(":");
    if(mins < 10) {
      tft.print("0");
    }
    tft.print(mins); tft.print(":");
    if(sec < 10) {
      tft.print("0");
    }
    tft.print(sec);
  }

}

void measureDraw() {
  // measure length of bubble event and draw rectangle based on length
  eventStart = millis();
  while(digitalRead(photoPin) == 0) {
    
  }
  eventEnd = millis();
  eventDuration = (eventEnd-eventStart) / 1000;
  //Serial.println(eventDuration);
  if(eventDuration >= 5) {
    rectangle(rectX, rectY, w, hCont, RED);
    rectX = rectX + 20;
  }
  else {
    rectangle(rectX, rectY, w, hInst, PURPLE);
    rectX = rectX + 20;
  }
  intFlag = 1;
}

void rectangle(int x0, int y0, int w, int h, int color) {
  tft.fillRect(x0, y0, w, h, color);
  tft.drawRect(x0, y0, w, h, WHITE);
}
