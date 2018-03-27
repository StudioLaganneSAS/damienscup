#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
//#include "Adafruit_TMP006.h"
#include "pitches.h"

Adafruit_MMA8451 mma = Adafruit_MMA8451();
//Adafruit_TMP006 tmp;

#define R 6
#define G 5
#define B 3
#define buzzer 12

float cupResistance = 9.00;
int   initMelody[] = { NOTE_C6, NOTE_D6, NOTE_E6 };

/*
int readTemp()
{

  return tmp.readObjTempC() + cupResistance;

}
*/

void setup(void)
{ 
 
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(buzzer, OUTPUT);

  //Serial.begin(9600);
  mma.begin();
  //tmp.begin();

  mma.setRange(MMA8451_RANGE_2_G);

  mma.read();
  for (int i = 0; i < 3; i++) {
    tone(buzzer, initMelody[i], 100);
    delay(100);
  }

}

/*
void reset()
{

  //Serial.println("RESET!");
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);
  moved     = false;    
  coffee    = false;
  tilt      = 0;
  cold      = 0;
  previousY = currentY;
  seconds   = 0;
  buzzed    = false;

}
*/

int   seconds   = 0;

bool  onWait = false;

int   waitReset = 300;


void loop()
{

  if (onWait) {
    if (seconds == waitReset) {
      onWait = false; 
      seconds = 0;
    }
  } else {
    mma.read();
    int x = mma.x;
    int y = mma.y;
    if (x > 2048) {
      digitalWrite(R, HIGH);
      tone(buzzer, NOTE_A4, 100);
      delay(100);
      digitalWrite(R, LOW);
    }
    if (y > 2048) {
      digitalWrite(G, HIGH);
      tone(buzzer, NOTE_B4, 100);
      delay(100);
      digitalWrite(G, LOW);
    }
  }

}

/*


bool  coffee    = false;
bool  moved     = false; 
bool  buzzed    = false;
int   tilt      = 0;
int   cold      = 0;

int   waitMove  = 30;
int   waitTilt  = 60;

void aloop() {

  if (!onWait) {
    int temp = readTemp();
    // If temperature is higher than 40 degrees, there's hot liquid!
    if (temp >= 45) {
      if (!coffee) {
        digitalWrite(R, HIGH);
        coffee = true;
      }      
      
      mma.read();
      currentY = mma.y;
      if (moved && tilt <= 4) {
        uint8_t orientation = mma.getOrientation();
  
        if (previousOrientation != orientation) {
          //Serial.println("TILT!");
          tilt++;
          previousOrientation = orientation;
          digitalWrite(B, HIGH);
        }
        
        if (seconds == waitTilt && tilt < 4) {
          tone(buzzer, NOTE_A4, 1000);
          delay(1000);
          tilt++;
          seconds = 0;
        }
      } else if (moved && tilt >= 4) {
        reset();
        onWait = true;
      } else {
        if (!buzzed && currentY - previousY >= 350) {
          if (!moved) {
            seconds = 0;
            //Serial.print("It's moving!"); Serial.println();
            moved = true;
            digitalWrite(G, HIGH);
          }
        } else {
          if (!buzzed && seconds == waitMove) {
            buzzed = true;
            tone(buzzer, NOTE_D4, 1000);
            delay(1000);
            seconds = 0;
            delay(1000);
            moved = true;
          }
        }
        previousY = currentY;    
      }
    } else {
      if (coffee) {
        // If it's cold for 3 seconds, we can assume it's a real reading...
        if (cold == 3 * 10) {
          reset();
        } else {
          // Increment until 3 readings in a row are cold.
          cold++;
        }
      }
    }
  } else {
    if (seconds == waitReset) {
      //Serial.println("WAIT IS OVER...");
      onWait  = false;
      reset();
    }
  }
  
  // Every 100 milliseconds...
  seconds++;
  delay(100);
  
}
*/
