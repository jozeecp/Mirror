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
  v0.0.2.1              7/26/17, added BNO055 code

  Current Version:
  v0.1.0                7/28/17, complete overhaul,
                                 uses local ip for mqtt broker
                                 "works"
                                 needs annotation
                                 no security
*/


#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
Adafruit_BNO055 bno = Adafruit_BNO055();


//EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "10.0.0.93"
const char* ssid = "Skynet";
const char* password = "Thanksjose!";

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
String macToStr(const uint8_t* mac);
unsigned long currentMillis = 0;

//LED on ESP8266 GPIO2
const int lightPin = 2;

char* lightTopic = "/test/light1";

int x = 0;
char msg4 = x;

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

// accelration variables
int xaccel = 0;
int yaccel = 0;
int zaccel = 0;
int accelGain = 10;

void setup() {
  // set up SDA on 5 and SCL on 4
  Wire.begin (5, 4);

  //start the serial line for debugging
  Serial.begin(115200);
  delay(100);

  // Initialise the sensor
  if(!bno.begin())
  {
    // There was a problem detecting the BNO055 ... check your connections
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }


  //start wifi subsystem
  WiFi.begin(ssid, password);
  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();

  //wait a bit before starting the main loop
      delay(2000);
}



void loop(){


  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

  //maintain MQTT connection
  client.loop();

  // function assigns accel values to variables
  accelValues(accelGain);

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10);
}


void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic;

  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);
}


void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED){
    //debug printing
    Serial.print("Connecting to ");
    Serial.println(ssid);

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    //print out some more debug once connected
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str())) {
        Serial.print("\tMTQQ Connected");
        client.subscribe(lightTopic);
      }

      //otherwise print failed for debugging
      else{Serial.println("\tFailed."); abort();}
    }
  }
}

//generate unique name from MAC addr
String macToStr(const uint8_t* mac){

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5){
      result += ':';
    }
  }

  return result;
}

void accelValues(int a) {
  // gyroscope function, angular velocity rads/sec
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  // assign acceleration values to variables
  xaccel = accel.x() * a;
  yaccel = accel.y() * a;
  zaccel = accel.z() * a;

  char x[10];
  dtostrf(xaccel,3,2,x);
  client.publish("/mirror/x", x);

  char y[10];
  dtostrf(yaccel,3,2,y);
  client.publish("/mirror/y", y);

  char z[10];
  dtostrf(zaccel,3,2,z);
  client.publish("/mirror/z", z);
}
