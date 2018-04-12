// Damien's Coffee Cup
//
// By John <psish> Chavarria
// Copyright (c) 2018 Studio Laganne, SAS http://www.studiolaganne.com
// MIT License
//
//


//
// Includes
//

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TMP006.h>
#include <pitches.h>


//
// Defines
//


// I2C Sensors
Adafruit_MMA8451 mma = Adafruit_MMA8451();
Adafruit_TMP006 tmp;


// ATMEGA328 pins
#define R       6
#define G       5
#define B       3
#define buzzer 12


// Initial X:Y values
int initialX, initialY;

// Temperature, Acceleration and Angles tolerances
int   moveTolerance  = 230; // Threshold for movement detection.
int   tempTolerance  = 48;  // It's considered coffee when it's 48 degrees.
float angleTolerance = 32;  // 32 degrees angle is a tilt.

// Timers
int seconds   = 1; // tic-toc, on the clock $$$
int movements = 0; // Number of movements detected so far
int tilts     = 0; // Number of tilts detected so far

// States
bool led     = false; // The LED is ON?
bool coffee  = false; // We got some beverage!!
bool moved   = false; // We made it past the coffee machine, yay!
bool tilting = false; // Are we stable, sir?! ARE WE TILTING?!
bool onBreak = false; // Don't do anything, we need a kit-kat

// Timers before buzzing
int timerMovement = 45; // We want to wait 45 seconds after coffee has been poured in and no movement has been detected
int timerTilting  = 60; // We want to wait 60 seconds in-between tilts
int tiltsAmount   = 5;  // Task fulfilment is done after 5 tilts has been made 

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
void buzz(bool fast = false) {

  for (int i = 0; i < (fast ? 5 : 40) ; i++) {

    led = !led;
    digitalWrite(R, led);

    if (i == 0 || i == 24) {
      
      tone(buzzer, (fast ? NOTE_A5 : NOTE_G3), 256);
    }

    if (i == 11 || i == 35) {

      tone(buzzer, (fast ? NOTE_A5 : NOTE_G3), 128);
    }
    
    delay(50);
  }
  
  digitalWrite(R, LOW);
  noTone(0);
}


// Inits the Accelerometer with values
void initMMA() {

  mma.read();
  
  initialX = mma.x;
  initialY = mma.y;
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


// Returns an acceleration event from the MMA readings
sensors_vec_t getAcceleration() {

  sensors_event_t event; 
  mma.getEvent(&event);

  return event.acceleration;
}


// Check if we're reached the max fulfilment of our task (5 tilts)
void checkFulfilment() {

  // If we tilted enough, reset and wait
  if (tilts >= tiltsAmount) {

    onBreak = true;
    reset();
  }
}


// Check if we're on a break, check if it's time to get back to work...
void checkBreak() {

  if (onBreak && seconds % breakTime == 0) {

     onBreak = false;
     seconds = 1;
  }
}


// Check if the contents of the cup is hot enough...
bool isItHot() {

  int temp = readTemp();
  return (temp >= tempTolerance);
}


// Check if the cup is moving...
bool isItMoving() {

  mma.read();

  int diffX = abs(mma.x - initialX); 
  int diffY = abs(mma.y - initialY); 

  // re-init
  initMMA();

  return ((diffX >= moveTolerance) || (diffY >= moveTolerance));
}


// Check if the cup is tilting...
bool isItTilting() {

  sensors_vec_t acceleration = getAcceleration();

  float angleX = angle(acceleration.x);
  float angleY = angle(acceleration.y);

  return ((angleX >= angleTolerance) || (angleY >= angleTolerance));
}


// Check if it's flat on the table...
bool isItFlat() {
  
  sensors_vec_t acceleration = getAcceleration();

  float angleX = angle(acceleration.x);
  float angleY = angle(acceleration.y);

  // Reset tilting if we're flat on the table again
  if (tilting && (angleX < 5.0 && angleY < 5.0)) {
    
    tilting = false;
  }
}




// *************************** //
//                             //
// Setup                       //
//                             //
// *************************** //

void setup() {

  // Arduino pins
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // TMP sensor init
  tmp.begin();

  // MMA Sensor init
  mma.begin();
  mma.setRange(MMA8451_RANGE_2_G);
  initMMA();

  // We're fucking ready
  buzz(true);
}




// *************************** //
//                             //
// Main Loop                   //
//                             //
// *************************** //

void loop() {

  // Are we on break? Should we go back to work?
  checkBreak();

  // Temperature Check
  bool isHot = isItHot();
  // Movement Check
  bool isMoving = isItMoving();
  // Inclination Check
  bool isTilting = isItTilting();

  // If something's hot, we blink RED once, we record we got coffee in.
  if (!onBreak && isHot) {

    if (!coffee) {

      coffee = true;
      seconds = 1;
      blink(1);
    }
  }

  // We have beverage in, we're moving so we shortly blink GREEN once and record we did move
  if (coffee && !moved && isMoving && !isTilting) {

    movements++;
    moved = true;
    seconds = 1;
    blink(2, true);
  }

  // Check if it's flat
  isItFlat();

  // We got beverage in, we're tilting o/ 
  if (coffee && moved && isTilting) {

    if (!tilting) {

      tilting = true;
      tilts++;
      seconds = 1;
      blink(3, true);
    }
  }

  // Buzzings...

  // If we haven't moved, check if it's time to buzz!
  if (coffee && !moved && movements < 1 && seconds % timerMovement == 0) {

    seconds = 1;
    buzz();
  }

  // If we're under tilt amount theshold, check if it's time to buzz!
  if (coffee && moved && tilts <= tiltsAmount && seconds % timerTilting == 0) {

    seconds = 1;
    buzz();
  }

  // Are we done?
  checkFulfilment();

  // Every seconds...
  seconds++;  
  delay(1000);
}
