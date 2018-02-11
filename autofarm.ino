#include "DHTesp.h"
#include <MicroGear.h>
#include <ESP8266WiFi.h>

const char* ssid     = "kidbright";
const char* password = "12345678";

#define APPID   "pondpoom"
#define KEY     "0Ru2v1x5H55jIZG"
#define SECRET  "xxVHEI3QmxM5JG9kokSV0Pacq"

#define ALIAS   "NodeMCU1"
#define TargetWeb "HTML_web"

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"
#define DHTPIN D2     // what digital pin we're connected to

#define WATER D0
#define LIGHT D1

DHTesp dht;

int numCounter = 0;


WiFiClient client;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}


void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) 
{
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}

int light;

void setup() {
  Serial.begin(250000);
  dht.setup(DHTPIN);
  Serial.println("DHT Setup!");
  
  /* Event listener */
  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);

  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
       delay(250);
       Serial.print(".");
    }

    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);

    Serial.println("NETPIE Connected : Done... ");

    //set Ouput Pin
    pinMode(WATER, OUTPUT);
    pinMode(LIGHT, OUTPUT);
    digitalWrite(WATER, LOW); 
    digitalWrite(LIGHT, LOW);
}

void loop() {
  // Wait a few seconds between measurements.
  //digitalWrite(LED_BUILTIN, LOW);
  //delay(1000);
  //digitalWrite(LED_BUILTIN, HIGH);
  light = analogRead(A0);
  Serial.print("Light : ");
  Serial.println(light);
  
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.print(dht.toFahrenheit(temperature), 1);
  Serial.print("\t\t");
  Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
  Serial.print("\t\t");
  Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);

  //open water if temp > 25
  if (temperature > 25) {
    digitalWrite(WATER, HIGH);
    Serial.println("Open Water!!!");
  } else {
    digitalWrite(WATER,LOW);
    Serial.println("Close Water!!!");
  }

  //open light if light meter <= 2
  if (light <= 2) {
    digitalWrite(LIGHT, HIGH);
    Serial.println("Open Light!!!");
  } else {
    digitalWrite(LIGHT,LOW);
    Serial.println("Close Light!!!");
  }
 

  if (microgear.connected())
    {
       microgear.loop();
       Serial.println("connected");

       int Humidity = (int)humidity;
       int Temp = (int)temperature;  // Read temperature as Celsius (the default)
       String data = String(Humidity) + "," + String(Temp) + "," + String(light);
      
       char msg[128];
       data.toCharArray(msg,data.length()+1);
       Serial.println(msg);    

       microgear.chat(TargetWeb , msg);
    }
   else 
   {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
   }

}
