#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Moter Shield Setup
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
int shallowTurnOutsideSpeed = 30;  // speed of the outside wheel during a shallow turn (should be higher than shallowTurnInsideSpeed)

int hardTurnInsideSpeed = 0;    // speed of the inside wheel during a hard turn (Black-Black-Light or Light-Black-Black)
int hardTurnOutsideSpeed = 30;  // speed of the outside wheel during a hard turn (should be higher than hardTurnInsideSpeed)

int forwardSpeed = 20; // 0 (stop) to 255 (full speed)


// Ask for speed tuning input (shallow turning only) at when starting? 
// input anytime by entering 2 in the serial input bar
bool getUserInput = false;

// Starting robot mode
int robotMode = 1; // 1 for allow motors to run, 0 for stop, 2 for get user input

// Initializing reused variables
unsigned long irL = 0.0; // will hold the current IR Sensor reading for sensor on the left (supposed to be off the line)
unsigned long irC = 0.0; // will hold the current IR Sensor reading for sensor in the center (supposed to be on the line)
unsigned long irR = 0.0; // will hold the current IR Sensor reading for sensor on the right (supposed to be off the line)
bool blackL = false; // Will be true if irL is above the limit set by irLimL
bool blackC = false; // Will be true if irL is above the limit set by irLimC
bool blackR = false; // Will be true if irL is above the limit set by irLimR
int newVal = 0; // for holding user input values for motor speeds
int moveDir = 0; // indicates in the serial what the robot is trying to do
                 // 0 for straight, 3 for stop, 4 for lost the line, 
                 // -2 for turn hard left, -1 for turn shallow left, 
                 // 1 for turn shallow right, 2 for turn hard right

void setup() {
  // put your setup code here, to run once:
  AFMS.begin();
  Serial.begin(9600); // open the serial port at 9600 bps:
}

void serial_flush(void) {
  // manually flushes the serial input so that user input values aren't misread
  while (Serial.available()) Serial.read();
}

void loop() {
  // put your main code here, to run repeatedly:

  // read values from the IR sensors
  irL = analogRead(A0);
  irC = analogRead(A1);
  irR = analogRead(A2);

  if (getUserInput){
    motorL->setSpeed(0); 
    motorL->run(FORWARD);
    motorR->setSpeed(0); 
    motorR->run(FORWARD);
    
    Serial.println("Get user input? Answer (and wait) in 5s for yes, ignore for no.");
    delay(5000);
    if (Serial.available() == 0) {
      getUserInput = false;
    } else {
      serial_flush();
    }
  }
  if (getUserInput){

    // Prompt user for input on shallowTurnInsideSpeed
    serial_flush();
    Serial.print("Supply shallowTurnInsideSpeed 0->255: (enter -1 to use pre-programmed value of "); 
    Serial.print(shallowTurnInsideSpeed); Serial.println(")");
    while (Serial.available() == 0) {
      // Wait for User to Input Data
    }
    newVal = Serial.parseInt(); // Read the data the user has input
    if ( newVal >= 0) {
      Serial.print("newVal: "); Serial.println(newVal);
      shallowTurnInsideSpeed = newVal;
    }

    // Prompt user for input on shallowTurnOutsideSpeed
    serial_flush();
    Serial.print("Supply shallowTurnOutsideSpeed 0->255: (enter -1 to use pre-programmed value of "); 
    Serial.print(shallowTurnOutsideSpeed); Serial.println(")");
    while (Serial.available() == 0) {
      // Wait for User to Input Data
    }
    newVal = Serial.parseInt(); // Read the data the user has input
    if ( newVal >= 0) {
      Serial.print("newVal: "); Serial.println(newVal);
      shallowTurnOutsideSpeed = newVal;
    }

    // Prompt user for input on forwardSpeed
    serial_flush();
    Serial.print("Supply forwardSpeed 0->255: (enter -1 to use pre-programmed value of "); 
    Serial.print(forwardSpeed); Serial.println(")"); 
    while (Serial.available() == 0) {
      // Wait for User to Input Data
    }
    newVal = Serial.parseInt(); // Read the data the user has input
    if ( newVal >= 0) {
      Serial.print("newVal: "); Serial.println(newVal);
      forwardSpeed = newVal;
    }

    getUserInput = false; // so that it skips user imput in future loops
  }


  // define the booleans for control loop readability
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

  // Robot control loop for line following
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

  // check the serial input for user commands
  if (Serial.available() > 0 ) {
    robotMode = Serial.parseInt();
  }
  
  // if robotMode is 0 then stop motors, if 2 then get new user inputs, 
  // (if 1 then drive motors at speed set in control loop)
  if (robotMode == 0) {
    motorL->setSpeed(0); 
    motorR->setSpeed(0); 
  } else if (robotMode == 2) {
    getUserInput = true;
    robotMode = 0;
  } 

  // send speeds to motors
  motorL->run(FORWARD);
  motorR->run(FORWARD);

  // print robotMode, irL, irC, irR, and moveDir to serial output for debugging
  Serial.print(robotMode);    Serial.print(",");
  Serial.print(irL);    Serial.print(",");
  Serial.print(irC);    Serial.print(",");
  Serial.print(irR);    Serial.print(",");
  Serial.println(moveDir);

}
