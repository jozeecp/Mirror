/*  By Jose Catarino
  Receives data from  sensors to be used as position input and sends it to
  another Adafruit Huzzah through WiFi for controlling the arm

  Components used:
      Adafruit Huzzah: https://www.adafruit.com/product/2471
      BNO055 motion sensor: https://www.adafruit.com/product/2472

  Previous versions:
  v0.0                  5/16/17, started document
  v0.0.1                7/25/17, first version
  v0.0.2                7/25/17, first working versions
                                 achieved data transfer to IO feed

  Current Version:
  v0.0.2.1              7/26/17, added BNO055 code
*/

// include Libraries
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
Adafruit_BNO055 bno = Adafruit_BNO055();

// WiFi parameters
#define WLAN_SSID     "Skynet"      //WLAN NAME
#define WLAN_PASS     "Thanksjose!" //WLAN PASSWORD

// Adafruit IO
#define AIO_SERVER    "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME   "jfcatarin" //Your Adafruit IO user name
#define AIO_KEY        "924eede090d14869bb58514f0d84cb35" //Your IO key

// Functions
void connect();

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;



// Set a unique MQTT client ID using the AIO key + the date and time the sketch
// was compiled (so this should be unique across multiple devices for a user,
// alternatively you can manually set this to a GUID or other random value).
const char MQTT_CLIENTID[] PROGMEM  = AIO_KEY __DATE__ __TIME__;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Set up feeds
Adafruit_MQTT_Publish xAccel = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/x-accel");
Adafruit_MQTT_Publish yAccel = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/y-accel");
Adafruit_MQTT_Publish zAccel = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/z-accel");

// accelration variables
int xaccel = 0;
int yaccel = 0;
int zaccel = 0;
int accelGain = 1;

void setup() {

  // set up SDA on 5 and SCL on 4
  Wire.begin (5, 4);

  Serial.begin(115200);

  // Initialise the sensor
  if(!bno.begin())
  {
    // There was a problem detecting the BNO055 ... check your connections
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  // connect to adafruit io
  connect();
}

void loop () {

  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
   }

   // function assigns accel values to variables
   accelValues(accelGain);
   // makes sure that values publish
   accelPublish();

}

void connect() {

 Serial.print(F("Connecting to Adafruit IO... "));

 int8_t ret;

 while ((ret = mqtt.connect()) != 0) {

   switch (ret) {
     case 1: Serial.println(F("Wrong protocol")); break;
     case 2: Serial.println(F("ID rejected")); break;
     case 3: Serial.println(F("Server unavail")); break;
     case 4: Serial.println(F("Bad user/pass")); break;
     case 5: Serial.println(F("Not authed")); break;
     case 6: Serial.println(F("Failed to subscribe")); break;
     default: Serial.println(F("Connection failed")); break;
   }

   if(ret >= 0)
     mqtt.disconnect();

   Serial.println(F("Retrying connection..."));
   delay(5000);

 }

 Serial.println(F("Adafruit IO Connected!"));
}

void accelValues(int a) {
  // gyroscope function, angular velocity rads/sec
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  // assign acceleration values to variables
  xaccel = accel.x() * a;
  yaccel = accel.y() * a;
  zaccel = accel.z() * a;

}

void accelPublish() {

  if (! xAccel.publish(xaccel))
   Serial.println(F("Failed to publish xAccel"));
  else
   Serial.println(F("xAccel published!"));
  delay(500);

  if (! yAccel.publish(yaccel))
   Serial.println(F("Failed to publish yAccel"));
  else
   Serial.println(F("yAccel published!"));
  delay(500);

  if (! zAccel.publish(zaccel))
   Serial.println(F("Failed to publish zAccel"));
  else
   Serial.println(F("zAccel published!"));
  delay(500);
}

void orientationValues() {

}
