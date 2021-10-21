#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorL = AFMS.getMotor(1);
Adafruit_DCMotor *motorR = AFMS.getMotor(2);

// Reflectivity Sensor Output Limits
// if higher than the irLim, that sensor will be assumed to be non-reflective, or over the black line
// if lower than the irLim, that sensor will be assumed to be reflective, indicating it's over the blue floor
int irLimL = 600;
int irLimC = 600;
int irLimR = 650;


// Speed Tuning Controls
// speeds should be between 0 (stop) and 255 (full speed)

int shallowTurnInsideSpeed = 0;    // speed of the inside wheel during a shallow turn (Light-Light-Black or Black-Light-Light)
int shallowTurnOutsideSpeed = 50;  // speed of the outside wheel during a shallow turn (should be higher than shallowTurnInsideSpeed)

int hardTurnInsideSpeed = 0;    // speed of the inside wheel during a hard turn (Black-Black-Light or Light-Black-Black)
int hardTurnOutsideSpeed = 50;  // speed of the outside wheel during a hard turn (should be higher than hardTurnInsideSpeed)

int forwardSpeed = 50; // 0 (stop) to 255 (full speed)


unsigned long irL = 0.0;
unsigned long irC = 0.0;
unsigned long irR = 0.0;

bool blackL = false;
bool blackC = false;
bool blackR = false;

bool getUserInput = false;
int newVal = 0;

int motorsGo = 1; // 1 for allow motors to run, 0 for stop.

int moveDir = 0; // 0 for straight, 3 for stop, 4 for lost the line, -2 for turn hard left, -1 for turn shallow left, 1 for turn shallow right, 2 for turn hard right

void setup() {
  // put your setup code here, to run once:
  AFMS.begin();
  Serial.begin(9600); // open the serial port at 9600 bps:
}

void loop() {
  // put your main code here, to run repeatedly:

  irL = analogRead(A0);
  irC = analogRead(A1);
  irR = analogRead(A2);

  if (getUserInput){
    motorL->setSpeed(0); 
    motorL->run(FORWARD);
    motorR->setSpeed(0); 
    motorR->run(FORWARD);
    
    Serial.println("Get user input? Answer in the next 5s for yes, ignore for no.");
    delay(5000);
    if (Serial.available() == 0) {
      getUserInput = false;
    }
  }
  if (getUserInput){

    Serial.print("Supply shallowTurnInsideSpeed 0->255: (enter -1 to use pre-programmed value of "); Serial.print(shallowTurnInsideSpeed); Serial.println(")"); //Prompt User for Input
    Serial.println(Serial.available());
    while (Serial.available() == 0) {
      Serial.println(Serial.available());
      // Wait for User to Input Data
    }
    newVal = Serial.parseInt(); //Read the data the user has input
    if ( newVal >= 0) {
      Serial.print("newVal: "); Serial.println(newVal);
      shallowTurnInsideSpeed = newVal;
    }

    Serial.print("Supply shallowTurnOutsideSpeed 0->255: (enter -1 to use pre-programmed value of "); Serial.print(shallowTurnOutsideSpeed); Serial.println(")"); //Prompt User for Input
    Serial.println(Serial.available());
    while (Serial.available() == 0) {
      Serial.println(Serial.available());
      // Wait for User to Input Data
    }
    newVal = Serial.parseInt(); //Read the data the user has input
    if ( newVal >= 0) {
      Serial.print("newVal: "); Serial.println(newVal);
      shallowTurnOutsideSpeed = newVal;
    }

    Serial.print("Supply forwardSpeed 0->255: (enter -1 to use pre-programmed value of "); Serial.print(forwardSpeed); Serial.println(")"); //Prompt User for Input
    while (Serial.available() == 0) {
      Serial.println(Serial.available());
      // Wait for User to Input Data
    }
    newVal = Serial.parseInt(); //Read the data the user has input
    if ( newVal >= 0) {
      Serial.print("newVal: "); Serial.println(newVal);
      forwardSpeed = newVal;
    }

    getUserInput = false;
  }


  if (irL > irLimL) {
    blackL = true;
  } else {
    blackL = false;
  }
  if (irC > irLimC) {
    blackC = true;
  } else {
    blackC = false;
  }
  if (irR > irLimR) {
    blackR = true;
  } else {
    blackR = false;
  }
  
  if (!blackL && blackC && !blackR) {             // <--  go straight (Light-Black-Light)
    motorL->setSpeed(forwardSpeed); 
    motorR->setSpeed(forwardSpeed); 
    moveDir = 0;
  } else if ( (blackL && blackC && !blackR) ) {   // <-- turn hard left (Black-Black-Light)
    motorL->setSpeed(hardTurnInsideSpeed); 
    motorR->setSpeed(hardTurnOutsideSpeed); 
    moveDir = -2;
  } else if ( (blackL && !blackC && !blackR) ) {  // <-- turn shallow left (Black-Light-Light)
    motorL->setSpeed(shallowTurnInsideSpeed); 
    motorR->setSpeed(shallowTurnOutsideSpeed); 
    moveDir = -1;
  } else if ( (!blackL && !blackC && blackR) ) {  // <--  turn shallow right (Light-Light-Black)
    motorL->setSpeed(shallowTurnOutsideSpeed); 
    motorR->setSpeed(shallowTurnInsideSpeed); 
    moveDir = 1;
  } else if ( (!blackL && blackC && blackR) ) {   // <-- turn hard right (Light-Black-Black)
    motorL->setSpeed(hardTurnOutsideSpeed); 
    motorR->setSpeed(hardTurnInsideSpeed); 
    moveDir = 2;
  } else if ( (!blackL && !blackC && !blackR)) {  // <-- the line has been lost (Light-Light-Light)
    motorL->setSpeed(shallowTurnOutsideSpeed); 
    motorR->setSpeed(shallowTurnInsideSpeed); 
    moveDir = 4;
  } else {                                        // otherwise stop (Black-Black-Black)
    motorL->setSpeed(0); 
    motorR->setSpeed(0); 
    moveDir = 3;
  }

  if (Serial.available() > 0 ) {
    motorsGo = Serial.parseInt();
  }

  if (motorsGo == 0) {
    motorL->setSpeed(0); 
    motorR->setSpeed(0); 
    motorL->run(FORWARD);
    motorR->run(FORWARD);
  } else {
    motorL->run(FORWARD);
    motorR->run(FORWARD);
  }

  Serial.print(motorsGo);    Serial.print(",");
  Serial.print(irL);    Serial.print(",");
  Serial.print(irC);    Serial.print(",");
  Serial.print(irR);    Serial.print(",");
  Serial.println(moveDir);
  

}
