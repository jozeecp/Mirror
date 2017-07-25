/**********************************************************
 
 Adafruit Huzzah IoT weather station with:
 
 DHT11  - For humidity
 BMP180 - For pressure
 TMP102 - For temperature

 I2C - pins - 5(SDA) and 4(SCL)
 
 DHT pin - 2

 by KonstatinDimitrov
***********************************************************/ 

/*******************LIBRARIES**************************/
// Libraries
#include <ESP8266WiFi.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

/********************SENSORS***************************/
//TMP102 sensor address
int tmp102Address = 0x48;

//Pressure sensor
SFE_BMP180 pressure1; 
char status;
double T,P,p0,a;
#define ALTITUDE 44.80

//DHT11 sensor
#define DHTPIN 2
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE, 15);
/********************SLEEP MODE**********************/
const int sleepTimeS = 300;    // Sleep mode time in seconds
/* 60s - 1min, 120s- 2min, 180s - 3min 300s - 5min */
/****************************************************/
// WI-FI PARAMETERS
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
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);/****************************** Feeds ***************************************/

/******************** Setup feeds ***********************/
const char TEMPERATURE_FEED[] PROGMEM = AIO_USERNAME "/feeds/temperature";
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, TEMPERATURE_FEED);

const char PRESSURE_FEED[] PROGMEM = AIO_USERNAME "/feeds/pressure";
Adafruit_MQTT_Publish pressure = Adafruit_MQTT_Publish(&mqtt, PRESSURE_FEED);

const char HUMIDITY_FEED[] PROGMEM = AIO_USERNAME "/feeds/humidity";
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, HUMIDITY_FEED);


/***************************Sketch Code******************/
void setup() {
  // 
  Wire.begin (5, 4); // setting up the SDA(5) and SCL(4) pins

  dht.begin();       //reads data from DHT sensor
 
  pressure1.begin(); //reads data from BMP180

  Serial.begin(115200);
  Serial.println(F("IoT weather station"));

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

/***********************TMP102 BLOCK********************/
 float getTemperature()
{
  Wire.requestFrom(tmp102Address,2); 
 
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  
  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 
  
  float TempC = TemperatureSum*0.0625;
  if (TempC > 128)
  { 
  TempC = TempC - 256; // for negative temperatures
  }
  else
    TempC;
  return TempC;
}
/*******************************************************/
void loop() {
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
   }

/***********************TMP102*******************/
   float TempC = getTemperature();

  if (! temperature.publish(TempC))
   Serial.println(F("Failed to publish temperature"));
  else
   Serial.println(F("Temperature published!"));
  delay(500);

/**********************Pressure******************/
 status = pressure1.startPressure(3);
 
 status = pressure1.getPressure(P,T);
 
 if (! pressure.publish(P*0.01450377))
   Serial.println(F("Failed to publish pressure"));
  else
   Serial.println(F("Pressure published!"));
  delay(500);

/******************DHT Humidity*******************/
int humidity_data = (int)dht.readHumidity();

 if (! humidity.publish(humidity_data))
    Serial.println(F("Failed to publish humidity"));
  else
    Serial.println(F("Humidity published!"));
    delay(500);

/*******************SLEP MODE***********************/    
    Serial.println(F("ENTERING SLEEP MODE!"));
    ESP.deepSleep(sleepTimeS * 1000000); //Sleep mode, for power save
}
/**************************************************/
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

        /*************** END OF FILE ******************/
