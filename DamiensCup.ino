// Studio Laganne 2018
// Damien's Coffee Cup
//
// By John <psish> Chavarria
//


//
// Includes
//

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TMP006.h"
#include "pitches.h"


//
// Variables
//


// I2C Sensors
Adafruit_MMA8451 mma = Adafruit_MMA8451();
Adafruit_TMP006 tmp;

// DEBUG
#define _DEBUG 0

// ATMEGA pins
#define R       6
#define G       5
#define B       3
#define buzzer 12

// Initial values
int initialX, initialY;

// Temperature, Acceleration and Angles tolerances
int   moveTolerance  = 200; // Threshold for movement detection.
int   tempTolerance  = 48;  // It's considered coffee when it's 48 degrees.
float angleTolerance = 32;  // 32 degrees angle is a tilt.

// Timer
int seconds      = 1; // tic-toc, on the clock $$$
int movements    = 0; // Number of movements detected so far
int tilts        = 0; // Number of tilts detected so far

// States
bool led     = false; // The LED is ON?
bool coffee  = false; // We got some beverage!!
bool moved   = false; // We made it past the coffee machine, yay!
bool tilting = false; // Are we stable, sir?! ARE WE TILTING?!
bool onBreak = false; // Don't do anything, we need a kit-kat

// Timers before buzzing
int timerMovement = 45;
int timerTilting  = 60;
int tiltsAmount   = 5;

// Break time
int breakTime  = 5*60; // kit-kat eating contest time (how long we pause before next work...)


//
// Functions
//


// Switches the LED ON or OFF
// colors: 0 = OFF | 1 = RED | 2 = GREEN | 3 = BLUE | 4 = WHITE
void light(int color) {

  switch (color) {
    
    case 0:
    
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
      break;   
    case 1:
      
      digitalWrite(R, HIGH);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
      break;
    case 2:
      
      digitalWrite(R, LOW);
      digitalWrite(G, HIGH);
      digitalWrite(B, LOW);
      break;
    case 3:
      
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, HIGH);
      break;
    case 4:
      
      digitalWrite(R, HIGH);
      digitalWrite(G, HIGH);
      digitalWrite(B, HIGH);
      break;
  }
}

// Blinks the LED really fast
void blink(int color, bool fast = false) {

  for (int i = 0; i < (fast ? 5 : 20); i++) {

    led = !led;
    light(led ? color : 0);
    delay(50);
  }
  light(0);
}

// Buzzes the Buzzer Mr. Buzzed
void buzz(bool shrt = false) {

  for (int i = 0; i < (shrt ? 5 : 40) ; i++) {

    led = !led;
    digitalWrite(R, led);

    if (i == 0 || i == 24) {
      
      tone(buzzer, (shrt ? NOTE_A5 : NOTE_G3), 256);
    }

    if (i == 11 || i == 35) {

      tone(buzzer, (shrt ? NOTE_A5 : NOTE_G3), 128);
    }
    
    delay(50);
  }
  
  digitalWrite(R, LOW);
  noTone(0);
}

// Inits the Accelerometer with values
void initMMA(bool pr = true) {

  mma.read();
  
  initialX = mma.x;
  initialY = mma.y;
  
  if (pr) {
   
    #if _DEBUG
      Serial.print("Init X : "); Serial.println(initialX);
      Serial.print("Init Y : "); Serial.println(initialY);
      Serial.println();
    #endif
  }
}

// Returns an angle from acceleration values
float angle(float acceleration) {

  return asin(abs(acceleration)/9.8) * 60;
}

// Read the object contactless temperature
int readTemp() {

  return tmp.readObjTempC() + 9.00;
}

// reset all the things
void reset() {

  seconds    = 1;

  movements  = 0;
  tilts      = 0;
  
  coffee     = false;
  moved      = false;
  tilting    = false;

}

// Arduino Setup
void setup() {

  // Arduino pins
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Serial Monitor in debug
  #if _DEBUG
    Serial.begin(9600);
  #endif

  // TMP sensor init
  tmp.begin();

  // MMA Sensor init
  mma.begin();
  mma.setRange(MMA8451_RANGE_2_G);
  initMMA(true);

  // We're fucking ready
  buzz(true);
}

// Main
void loop() {

  // If we're on a break, check if it's time to get back to work...
  if (onBreak && seconds % breakTime == 0) {

     onBreak = false;
     seconds = 1;
  }

  // Temperature Check
  int temp = readTemp();
  bool isHot = (temp >= tempTolerance);

 // Movement Check
  mma.read();

  int x = mma.x;
  int y = mma.y;
  int z = mma.z;
  
  int diffX = abs(x - initialX); 
  int diffY = abs(y - initialY); 

  bool isMoving = ((diffX >= moveTolerance) || (diffY >= moveTolerance));
  
  // Inclination Check
  sensors_event_t event; 
  mma.getEvent(&event);

  float angleX = angle(event.acceleration.x);
  float angleY = angle(event.acceleration.y);

  bool isTilting = ((angleX >= angleTolerance) || (angleY >= angleTolerance));
  bool isFlat    = ((angleX < angleTolerance)  && (angleY < angleTolerance ));
  
  #if _DEBUG
    Serial.println();
    Serial.print("X : "); Serial.print(diffX); Serial.print(" / "); Serial.println(angleX);
    Serial.print("Y : "); Serial.print(diffY); Serial.print(" / "); Serial.println(angleX);
    Serial.println();
  #endif
    
  initMMA();

  // If something's hot, we blink RED once, we record we got coffee and in and save the time we detected it
  if (!onBreak && isHot) {

    if (!coffee) {

      coffee = true;
      blink(1);
    }
  }

  // We have beverage in, we're moving so we shortly blink GREEN once and record we did move
  if (coffee && !moved && isMoving && !isTilting) {

    movements++;
    moved = true;
    blink(2, true);
  }

  // Reset tilting if we're flat on the table again
  if (tilting && isFlat) {
    
    tilting = false;
  }

  // We got beverage in, we're tilting o/ 
  if (coffee && moved && isTilting) {

    if (!tilting) {

      tilting = true;
      tilts++;
      blink(3, true);
    }
  }

  // If we haven't moved, check if it's time to buzz!
  if (coffee && movements < 1 && seconds % timerMovement == 0) {

    seconds = 0;
    buzz();
  }

  // If we're under tilt amount theshold, check if it's time to buzz!
  if (coffee && tilts <= tiltsAmount && seconds % timerTilting == 0) {

    seconds = 0;
    buzz();
  }

  seconds++;

  // If we tilted enough, reset and wait
  if (tilts >= tiltsAmount) {

    onBreak = true;
    reset();
  }
  
  delay(1000);
}

