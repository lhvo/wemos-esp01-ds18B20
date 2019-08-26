
// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "SSID";
const char* password = "PWD";
const char* sensorName ="esp01-01";

const int duration = 300; /* duration in sec */

float h=0;
float t =0;
float hic=0;


void getData() {


  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
   Serial.print("Temperature for the device 1 (index 0) is: ");
  t=sensors.getTempCByIndex(0);
  h=0;
  Serial.println(t);  
  
}



void prepareSensor() {  
sensors.begin();
}

void wifiOff(){
  WiFi.disconnect(); 
  WiFi.mode(WIFI_OFF);delay(1);
  
}
void initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }

  if (MDNS.begin(sensorName)) {
    Serial.println("MDNS responder started");
  }
  
}
void setup(void){
  
  Serial.begin(115200);
  prepareSensor();  
  

}

void sendData() {
       unsigned int vcc=0;
       String message ="weather,sensor="""+String(sensorName)+""" humidity="+String(h)+",temperature="+String(t)+",heatIndex=0,vcc="+String(vcc);
       HTTPClient http;
       http.begin("http://pi1.local:8086/write?db=db");
       http.addHeader("Content-Type", "application/x-www-form-urlencoded");
       int httpCode=http.POST(message);
       Serial.println(message);
       http.end();
 
}
void loop(void){
  unsigned long t;
  t=millis();
  initWifi();
  getData();
  sendData();
  wifiOff();
  delay(duration * 1e3 - (millis()-t));

}
