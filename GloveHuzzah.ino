/*  By Jose Catarino
  Receives data from  sensors to be used as position input and sends it to
  another Adafruit Huzzah though WiFi for controlling the arm

  Components used:
      Adafruit Huzzah: https://www.adafruit.com/product/2471
      BNO055 motion sensor: https://www.adafruit.com/product/2472

  Previous versions:
    N/A

  Current Version:
  v0.0                  5/16/17, started document
*/

// include Libraries
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Adafruit_BNO055.h>

// WiFi parameters
#define WLAN_SSID     "NAME"      //WLAN NAME
#define WLAN_PASS     "PASSWORD" //WLAN PASSWORD

// Adafruit IO
#define AIO_SERVER    "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME   "USERNAME" //Your Adafruit IO user name
#define AIO_KEY        "YOU KEY HERE" //Your IO key

// Functions
void connect();

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Store the MQTT server, client ID, username, and password in flash memory.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;

// Set a unique MQTT client ID using the AIO key + the date and time the sketch
// was compiled (so this should be unique across multiple devices for a user,
// alternatively you can manually set this to a GUID or other random value).
const char MQTT_CLIENTID[] PROGMEM  = AIO_KEY __DATE__ __TIME__;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);

// Set up feeds

void setup() {
  Wire.begin (5, 4); // set up SDA on 5 and SCL on 4

  // Initialise the sensor
  if(!bno.begin())
  {
    // There was a problem detecting the BNO055 ... check your connections
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  Serial.begin(115200);

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
