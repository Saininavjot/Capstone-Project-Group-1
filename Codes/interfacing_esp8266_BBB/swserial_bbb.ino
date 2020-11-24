#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>

#ifndef D5
#define D5 (14)
#define D6 (12)
#define TX (1)
#endif
#define BAUD_RATE 9600

int val;
//SSID of your network
char ssid[] = "Danish";
//password of your WPA Network
char password[] = "4164346666";
const int ledPin =  LED_BUILTIN;      // the number of the LED pin


SoftwareSerial swSer;   //object of a structure

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

// Important: the buffer size optimizations here, in particular the isrBufSize (11) that is only sufficiently
  // large to hold a single 8N1 word, are on the basis that any char written to the loopback SoftwareSerial
  // adapter gets read before another write is performed.
  // Block writes with a size greater than 1 would usually fail.
  swSer.begin(BAUD_RATE, SWSERIAL_8N1, D5, D6, false, 95, 11);


  pinMode(ledPin, OUTPUT); 
  WiFi.mode(WIFI_STA);    //wife mode, station and hotspot
  WiFi.begin(ssid, password);       //setting user name and password and connecting to wifi
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);         //waiting to make a connection
    
  Serial.print(".");
  }
Serial.println("Wifi Connected!!");
}


void loop() {
  // put your main code here, to run repeatedly:
  if(swSer.available())
  {
 val = swSer.read();
 yield(); 
 if(val == 65)
 {
    digitalWrite(ledPin, LOW);
  
     long rssi = WiFi.RSSI();
     Serial.print("RSSI:");
     Serial.println(rssi);
 }
 
 else if (val == 66)
 {
   digitalWrite(ledPin,HIGH);
 }
}
}
