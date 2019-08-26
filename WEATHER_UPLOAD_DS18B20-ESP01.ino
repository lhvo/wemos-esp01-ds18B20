/*
  HelloServerSecure - Simple HTTPS server example

  This example demonstrates a basic ESP8266WebServerSecure HTTPS server
  that can serve "/" and "/inline" and generate detailed 404 (not found)
  HTTP respoinses.  Be sure to update the SSID and PASSWORD before running
  to allow connection to your WiFi network.

  IMPORTANT NOTES ABOUT SSL CERTIFICATES

  1. USE/GENERATE YOUR OWN CERTIFICATES
    While a sample, self-signed certificate is included in this example,
    it is ABSOLUTELY VITAL that you use your own SSL certificate in any
    real-world deployment.  Anyone with the certificate and key may be
    able to decrypt your traffic, so your own keys should be kept in a
    safe manner, not accessible on any public network.

  2. HOW TO GENERATE YOUR OWN CERTIFICATE/KEY PAIR
    A sample script, "make-self-signed-cert.sh" is provided in the
    ESP8266WiFi/examples/WiFiHTTPSServer directory.  This script can be
    modified (replace "your-name-here" with your Organization name).  Note
    that this will be a *self-signed certificate* and will *NOT* be accepted
    by default by most modern browsers.  They'll display something like,
    "This certificate is from an untrusted source," or "Your connection is
    not secure," or "Your connection is not private," and the user will
    have to manully allow the browser to continue by using the
    "Advanced/Add Exception" (FireFox) or "Advanced/Proceed" (Chrome) link.

    You may also, of course, use a commercial, trusted SSL provider to
    generate your certificate.  When requesting the certificate, you'll
    need to specify that it use SHA256 and 1024 or 512 bits in order to
    function with the axTLS implementation in the ESP8266.

  Interactive usage:
    Go to https://esp8266-webupdate.local/firmware, enter the username
    and password, and the select a new BIN to upload.

  To upload through terminal you can use:
  curl -u admin:admin -F "image=@firmware.bin" esp8266-webupdate.local/firmware

  Adapted by Earle F. Philhower, III, from the HelloServer.ino example.
  This example is released into the public domain.
*/

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

const char* ssid = "dd-wrt_vap";
const char* password = "ff00ff00ff";
const char* sensorName ="esp01-01";
const char* table="test";
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
