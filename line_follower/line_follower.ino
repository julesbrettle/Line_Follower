#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorL = AFMS.getMotor(1);
Adafruit_DCMotor *motorR = AFMS.getMotor(2);

void setup() {
  // put your setup code here, to run once:
  AFMS.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

  motorL->setSpeed(150);
  motorL->run(FORWARD);

  

}
