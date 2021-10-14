#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorL = AFMS.getMotor(1);
Adafruit_DCMotor *motorR = AFMS.getMotor(2);

int irLimL = 920;
int irLimC = 935;
int irLimR = 940;

int turnInSpeed = 0;    // 0 (stop) to 255 (full speed)
int turnOutSpeed = 100;  // 0 (stop) to 255 (full speed)
int forwardSpeed = 200; // 0 (stop) to 255 (full speed)

unsigned long irL = 0.0;
unsigned long irC = 0.0;
unsigned long irR = 0.0;

bool blackL = false;
bool blackC = false;
bool blackR = false;

bool getUserInput = false;
int newVal = 0;

int moveDir = 0; // 0 for straight, -1 for turn left, 1 for turn right

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

  motorL->setSpeed(0); 
  motorL->run(FORWARD);
  motorR->setSpeed(0); 
  motorR->run(FORWARD);

  if (getUserInput){
    Serial.println("Get user input? Answer in the next 5s for yes, ignore for no.");
    delay(5000);
    if (Serial.available() == 0) {
      getUserInput = false;
    }
  }
  if (getUserInput){

    Serial.print("Supply turnInSpeed 0->255: (enter -1 to use pre-programmed value of "); Serial.print(turnInSpeed); Serial.println(")"); //Prompt User for Input
    Serial.println(Serial.available());
    while (Serial.available() == 0) {
      Serial.println(Serial.available());
      // Wait for User to Input Data
    }
    newVal = Serial.parseInt(); //Read the data the user has input
    if ( newVal >= 0) {
      Serial.print("newVal: "); Serial.println(newVal);
      turnInSpeed = newVal;
    }

    Serial.print("Supply turnOutSpeed 0->255: (enter -1 to use pre-programmed value of "); Serial.print(turnOutSpeed); Serial.println(")"); //Prompt User for Input
    Serial.println(Serial.available());
    while (Serial.available() == 0) {
      Serial.println(Serial.available());
      // Wait for User to Input Data
    }
    newVal = Serial.parseInt(); //Read the data the user has input
    if ( newVal >= 0) {
      Serial.print("newVal: "); Serial.println(newVal);
      turnOutSpeed = newVal;
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
  
  if (!blackL && blackC && !blackR) {                                                                                 // <-- situations in which to go straight
    motorL->setSpeed(forwardSpeed); 
    motorL->run(FORWARD);
    motorR->setSpeed(forwardSpeed); 
    motorR->run(FORWARD);
    moveDir = 0;
  } else if ( (blackL && blackC && !blackR) || (blackL && !blackC && !blackR) || (!blackL && !blackC && !blackR) ) {  // <-- situations in which to turn left
    motorL->setSpeed(turnInSpeed); 
    motorL->run(FORWARD);
    motorR->setSpeed(turnOutSpeed); 
    motorR->run(FORWARD);
    moveDir = -1;
  } else if ( (!blackL && blackC && blackR) || (!blackL && !blackC && blackR) ) {                                     // <-- situations in which to turn right
    motorL->setSpeed(turnOutSpeed); 
    motorL->run(FORWARD);
    motorR->setSpeed(turnInSpeed); 
    motorR->run(FORWARD);
    moveDir = 1;
  } else {                                                                                                            // otherwise stop
    motorL->setSpeed(0); 
    motorL->run(FORWARD);
    motorR->setSpeed(0); 
    motorR->run(FORWARD);
    moveDir = 2;
  }

  Serial.print(irL);    Serial.print(",");
  Serial.print(irC);    Serial.print(",");
  Serial.print(irR);    Serial.print(",");
  Serial.println(moveDir);
  

}
