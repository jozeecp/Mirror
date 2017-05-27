/*  By Jose Catarino
  Receives data from  sensors to be used as position input an communicates as a
  string to another Arduino in the base of the robot

  Components used:
      Arduino Mini Pro v5
      BNO055 motion sensor: https://www.adafruit.com/product/2472
      Rf transceiver NRF24L01: [link]

  Previous versions:
    v0.0                  5/16/17, started document


  Current Version:
    v0.0.1                5/18/17, added bno055 stuff
*/

// libraries needed for BNO055 position sensor
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h> // https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/overview
#include <utility/imumaths.h>
#include <Servo.h>
Adafruit_BNO055 bno = Adafruit_BNO055();

// libraries for rf sending
#include <VirtualWire.h>

// variables from linear acceleration
int x;
int y;
int z;
//variables for quat orientation
int qx;
int qy;
int qz;
// scaling factors
int scl = 1;
int qscl = 1;
// variables for rfSnd
int pos; // position data
char data[5]; // store pos data as a character

void setup() {
  Serial.begin(9600);
  // Initialise the sensor
  if(!bno.begin())
  {
    // There was a problem detecting the BNO055 ... check your connections
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

}

void loop() {
  BNO055input();
  rfSnd();
}

void BNO055input () {
  // reads input from BNO055 sensor

  // linear acceleration function, m/s^2
  imu::Vector<3> accl = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  // get absolute angle position
  imu::Quaternion quat = bno.getQuat();

  // assign linear acceleration to coordinate variables
  x = accl.x() * scl;
  y = accl.y() * scl;
  z = accl.z() * scl;

  // assign absolute position to variables
  qx = quat.x() * qscl;
  qy = quat.y() * qscl;
  qz = quat.z() * qscl;

}

void rfSnd () {
  // sends data thorugh rf module
  changeC();
}

void changeC(int num,char *data)
{
  //function to convert temperature data from integers to character array
  int k = num,i = 0,j;

  while (k > 0)
  {
    k/=10;
    i++;
  }
  int l = i;

  for (i = 0,j = l-1; i < l; i++,j--)
  {
    data[j] = char(num%10+48);
    num/=10;
  }
}
