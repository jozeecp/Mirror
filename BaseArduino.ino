/* By Jose Catarino
  Recieves data from sensor glove to control robot arm

  Components used:
      Arduino Uno
      Adafruit Servo Shield: https://www.adafruit.com/product/1411
      SG90 servo motors

  Previous versions:
    N/A

  Current Version:
  v0.0                  5/16/17, started document
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>   // for pwm shield
#include <Servo.h>                     // for servo testing

void setup() {

}

void loop () {

}

void servoI2C () {
  // writes data to servo motors usin teh I2C interface
}
void servoTest () {
  // for testing servos using regular servo output, aka no shield
}
