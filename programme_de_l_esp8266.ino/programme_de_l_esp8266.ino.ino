/***************************************************
Programme de transmission des données reçues par l'ESP de la part du teensy par la liaison série
 ****************************************************/
#include <ESP8266WiFi.h>
#include <stdio.h>
#include <WString.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

const char* WLAN_SSID = "OnePlus3 = 4";
const char* WLAN_PASS = "abcdefgbite328";

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "pjoubert"
#define AIO_KEY         "ba03eebced704f1b90bd0834fa28a223"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// définition des variables de l'API

Adafruit_MQTT_Subscribe timefeed = Adafruit_MQTT_Subscribe(&mqtt, "time/seconds");

Adafruit_MQTT_Publish pression = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pression", MQTT_QOS_1);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature", MQTT_QOS_1);
Adafruit_MQTT_Publish humidity_earth = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity_earth", MQTT_QOS_1);
Adafruit_MQTT_Publish humidity_air = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity_air", MQTT_QOS_1);
Adafruit_MQTT_Publish height = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/height", MQTT_QOS_1);
Adafruit_MQTT_Publish luminosity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/luminosity", MQTT_QOS_1);

// Setup a feed called 'onoff' for subscribing to changes to the button
//Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led_id", MQTT_QOS_1);

/*************************** Sketch Code ************************************/

int sec;
int min;
int hour;

int val_pression = 0;
int val_temperature = 0;
int val_humidity_earth = 0;
int val_humidity_air = 0;
int val_height = 0;
int val_luminosity = 0;


int timeZone = -4; // utc-4 eastern daylight time (nyc)
char buff[100];
int i = 0;
void timecallback(uint32_t current) {

  // adjust to local time zone
  current += (timeZone * 60 * 60);

  // calculate current time
  sec = current % 60;
  current /= 60;
  min = current % 60;
  current /= 60;
  hour = current % 24;

    Serial.print(hour);

  // print mins
  Serial.print(":");
  if(min < 10) Serial.print("0");
  Serial.print(min);

  // print seconds
  Serial.print(":");
  if(sec < 10) Serial.print("0");
  Serial.print(sec);

  if(hour < 12)
    Serial.println(" am");
  else
    Serial.println(" pm");

}

void slidercallback(double x) {
  Serial.print("Hey we're in a slider callback, the slider value is: ");
  Serial.println(x);
}

void onoffcallback(char *data, uint16_t len) {
  Serial.print("Hey we're in a onoff callback, the button value is: ");
  Serial.println(data);
}


int16_t read4bits(void ) {
  return ((Serial.read()&0xFF) << 8 | (Serial.read()&0xFF));
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
 
}

uint32_t x=0;

void loop() {
  
  
  if (Serial.available() > 0) {
    val_pression = (uint16_t)read4bits();
    val_temperature = read4bits();
    val_humidity_air = read4bits();
    val_luminosity = read4bits();
    val_height = read4bits();
    
    MQTT_connect();
    if(!pression.publish(val_pression)||!temperature.publish(val_temperature)||!humidity_earth.publish(val_humidity_earth)||!humidity_air.publish(val_humidity_air)||!luminosity.publish(val_luminosity)||!height.publish(val_height))
    {
    Serial.println(F("Failed"));
    } else {
      Serial.println(val_pression);
      Serial.println(val_temperature);
      Serial.println(val_humidity_earth);
      Serial.println(val_humidity_air);
      Serial.println(val_luminosity);
      Serial.println(val_height);
      Serial.println(F("Dac!"));
    }
    //mqtt.disconnect();
    }


    
    /*if( val_rec == 'p')
    {
      Serial.write(val_pression);
      val_pression = Serial.parseInt();
      if(pression.publish(val_pression))
      {
          Serial.write(0x0A);
      }
      
    
    } 
    if( val_rec == 't')
    {
      val_temperature = Serial.parseInt();
       Serial.write(val_temperature);
      if(temperature.publish(val_temperature))
      {
          Serial.write(0x0A);
      }
    } 
    if( val_rec == 'e')
    {
      val_humidity_earth = Serial.parseInt();
      Serial.write(val_humidity_earth);
      if(humidity_earth.publish(val_humidity_earth))
      {
          Serial.write(0x0A);
      }
    } 
    if( val_rec == 'a')
    {
      val_humidity_air = Serial.parseInt();
      Serial.write(val_humidity_air);
      if(humidity_air.publish(val_humidity_air))
      {
          Serial.write(0x0A);
      }
    }
    if( val_rec == 'l')
    {
      val_luminosity = Serial.parseInt();
      Serial.write(val_luminosity);
      if(luminosity.publish(val_luminosity))
      {
          Serial.write(0x0A);
      }
    } 
    if( val_rec == 'h')
    {
      val_height = Serial.parseInt();
      Serial.write(val_height);
      if(height.publish(val_height))
      {
          Serial.write(0x0A);
      }
    } 
  }
  */
  
  
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(10000);  // wait 10 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

