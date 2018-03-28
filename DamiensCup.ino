#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TMP006.h"
#include "pitches.h"
#define NOTE_00 0

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
int initialX, initialY, initialZ, temperature, timer = 0;

// Temperature and Acceleration tolerances
int tolerance     = 200;
int tempTolerance = 48;

// States
bool moved   = false;
int state    = 0;
int stateMax = 5;

// Timers
int sleep            = 2 * 60;
int waitForMovement  = 30;
int waitForTilt      = 60;
int sleepInitialMove = 20;
int sleepMove        = 5;

int readTemp()
{

  return tmp.readObjTempC() + 9.00;

}

void initMMA()
{

  mma.read();
  
  initialX = mma.x;
  initialY = mma.y;
  initialZ = mma.z;
  #if _DEBUG
    Serial.print("Init X : "); Serial.println(initialX);
    Serial.print("Init Y : "); Serial.println(initialY);
    Serial.print("Init Z : "); Serial.println(initialZ);
    Serial.println();
  #endif

}

void wait(int seconds = 1)
{

  delay(seconds * 1000);
  
}

bool atTimer(int t)
{

  return (timer > 0 && timer % t == 0);

}

void light(int color)
{

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

void buzz()
{

  int melody[][2] = {
    {NOTE_E3, 8},
    {NOTE_E3, 8},
    {NOTE_00, 8},
    {NOTE_E3, 8},
    {NOTE_00, 8},
    {NOTE_C3, 8},
    {NOTE_E3, 8},
    {NOTE_00, 8},
    {NOTE_G3, 8},
    {NOTE_00, 4},
    {NOTE_00, 8},
    {NOTE_G2, 8},
    {NOTE_00, 8},
    {NOTE_00, 4}
  };

  for (int note = 0; note < sizeof(melody)/4 - 1 ; note++) {
    int duration = 1000/melody[note][1];
    tone(buzzer, melody[note][0], duration);
    delay(duration * 1.30);
    noTone(8);
  }  

}

void setState(int s)
{

  #if _DEBUG
    Serial.print("Setting State at: "); Serial.println(s);
  #endif

  state = s;
  light(s);
  
  if (state == stateMax) {
    setState(0);
    reset();
    #if _DEBUG
      Serial.println(); Serial.println("... sleep ..."); Serial.println();
    #endif    
    wait(sleep);
  }
  
}

bool hasMoved()
{

  boolean moved = false;
  
  mma.read();
  int x     = mma.x;
  int y     = mma.y;
  int z     = mma.z;
  int diffX = abs(x - initialX); 
  int diffY = abs(y - initialY); 
  int diffZ = abs(z - initialZ); 
  if (diffX > tolerance) {
    moved = true;
  }
  if (diffY > tolerance) {
    moved = true;
  }
  if (diffZ > tolerance) {
    moved = true;
  }

  if (moved) {
    #if _DEBUG
      Serial.println();
      Serial.print("X : "); Serial.println(diffX);
      Serial.print("Y : "); Serial.println(diffY);
      Serial.print("Z : "); Serial.println(diffZ);
      Serial.println();
    #endif
    initMMA();
  }
    
  return moved;
  
}

void checkMovement()
{

  timer = 0;
  moved = false;

  int timing = waitForMovement;
  if (state > 1) {
    timing = waitForTilt;
  }
  
  initMMA();
  while(!atTimer(timing) && !moved) {
    moved = hasMoved();
    #if _DEBUG
      Serial.print(".");
    #endif
    timer++;
    wait();
  }

  if (moved) {
    #if _DEBUG
      Serial.println();
      Serial.println(); Serial.println(">>> has moved!"); Serial.println();
    #endif
  } else {
    buzz();
    #if _DEBUG
      Serial.println();
      Serial.println(); Serial.println("/!\\ FORGOT COFFEE /!\\"); Serial.println();
    #endif
  }

  setState(state + 1);

  int sleeping = sleepInitialMove;
  if (state > 1) {
    sleeping = sleepMove; 
  }
  wait(sleeping);

}

void setup()
{ 
 
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(buzzer, OUTPUT);

  #if _DEBUG
    Serial.begin(9600);
  #endif

  tmp.begin();
  temperature = readTemp();
  #if _DEBUG
    Serial.print("Initial Temperature: "); Serial.println(temperature);
  #endif
  
  mma.begin();
  mma.setRange(MMA8451_RANGE_2_G);
  initMMA();

  #if !_DEBUG
    buzz();
  #endif
  
}

void reset()
{

  timer = 0;
  temperature = readTemp();
  moved = false;
  
}

void loop()
{

  #if _DEBUG
    Serial.println("Ready...");
  #endif
  
  while(temperature < tempTolerance) {
    temperature = readTemp();
    wait();
  }

  setState(1);
  
  #if _DEBUG
    Serial.print("Temperature is at: "); Serial.println(temperature);
  #endif

  while (state > 0 && state <= stateMax) {
    checkMovement();
    wait();
  }
    
}
