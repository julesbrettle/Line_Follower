#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorL = AFMS.getMotor(1);
Adafruit_DCMotor *motorR = AFMS.getMotor(2);

int irLimL = 960;
int irLimC = 960;
int irLimR = 960;

int turnInSpeed = 0;
int turnOutSpeed = 50;
int forwardSpeed = 100;

bool blackL = false;
bool blackC = false;
bool blackR = false;

int moveDir = 0; // 0 for straight, -1 for turn left, 1 for turn right

void setup() {
  // put your setup code here, to run once:
  AFMS.begin();

  Serial.begin(9600); // open the serial port at 9600 bps:
  

}

void loop() {
  // put your main code here, to run repeatedly:

  unsigned long irL = analogRead(A0);
  unsigned long irC = analogRead(A1);
  unsigned long irR = analogRead(A2);
  
  Serial.print(irL);    Serial.print(",");
  Serial.print(irC);    Serial.print(",");
  Serial.println(irR);

  motorL->setSpeed(0); // 0 (stop) to 255 (full speed)
  motorL->run(FORWARD); 

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
  
  if (!blackL && blackC && !blackR) {
    
  }

  
  

  switch(moveDir) {
    case 0: 
      motorL->setSpeed(forwardSpeed); // 0 (stop) to 255 (full speed)
      motorL->run(FORWARD);
      motorR->setSpeed(forwardSpeed); // 0 (stop) to 255 (full speed)
      motorR->run(FORWARD);
      break;
    case -1:
      motorL->setSpeed(turnInSpeed); // 0 (stop) to 255 (full speed)
      motorL->run(FORWARD);
      motorR->setSpeed(turnOutSpeed); // 0 (stop) to 255 (full speed)
      motorR->run(FORWARD);
      break;
    case 1:
      motorL->setSpeed(turnOutSpeed); // 0 (stop) to 255 (full speed)
      motorL->run(FORWARD);
      motorR->setSpeed(turnInSpeed); // 0 (stop) to 255 (full speed)
      motorR->run(FORWARD);
      break;
  }

  

  

}
