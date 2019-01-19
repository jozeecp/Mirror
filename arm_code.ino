/* arm_code.ino by Jose Catarino
  This code controls a PRRRR 5 DOF robot arm
  
  Connections:
  L293D and Stepper Motor:
        +5V               1|      |9     +5V
        Pin 2             2|      |10    Pin 7
        Stepper green     3|      |11    Stepper red
        GND               4|      |12    GND
        GND               5|      |13    GND
        Stepper black     6|      |14    Stepper blue
        Pin 4             7|      |15    Pin 6
        +5V               8|      |16    +5V

  Current Version:
    v0.0                   // stepper motor control -- works!, 1/17/2019
  
  Previous Version:
    
*/


// Stepper Set Up
#include <AccelStepper.h>    // http://www.airspayce.com/mikem/arduino/AccelStepper/index.html
#define HALF4WIRE 8

// pins for the L293D for stepper motor
#define motorPin1  2
#define motorPin2  4
#define motorPin3  6
#define motorPin4  7
AccelStepper stepper1 (HALF4WIRE, motorPin1, motorPin2, motorPin3, motorPin4, true);

// some variables
int g; // robot speed

void setup() {
  // Initialise the serial monitor
  Serial.begin(9600);
  
  // Initialise pot pin
  pinMode(A0,INPUT);
  
  // Initialise stepper motor values
  stepper1.setMaxSpeed(1000);
  stepper1.setSpeed(500);
}

void loop() {
  g = analogRead(A0);
  g = map(g, 0,1023,-800,800);

  Serial.println(g);
  
  // filter
  if(g <= 30 && g >= -30)  {
    stepper1.setSpeed(0);
  }
  //set motor speed
  else{
    stepper1.setSpeed(-g);
  }
  // start the motor
  stepper1.runSpeed();
}
