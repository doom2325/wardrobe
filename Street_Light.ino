# wardrobe
#include <UbidotsMicroESP8266.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiUdp.h>

/*
 Requirements:
 
   * ESP8266 Wi-Fi Device
   * Arduino 1.6.9+ IDE
   * Additional Boards URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
   * Library: esp8266 by ESP8266 Community
 
 ThingSpeak Setup:
 
   * Sign Up for New User Account - https://thingspeak.com/users/sign_up
   * Create a new Channel by selecting Channels, My Channels, and then New Channel
   * Enable one field
   * Note the Channel ID and Write API Key
    
 Setup Wi-Fi:
  * Enter SSID
  * Enter Password
  
   
 Created: Feb 20, 2018 by Akshansh Jain
*/ 

// ADC Reference voltage
#define AREF_VOLTAGE   9.0
// Resistance of fixed resistor in LDR circuit (should be measured for better accuracy)
#define REF_RESISTANCE 10000.0

float ledpin();
String floatToString(double number, uint8_t digits);
// Wi-Fi Settings
const char* ssid = "Anxirex"; // your wireless network name (SSID)
const char* password = "anxirex9597"; // your Wi-Fi network password

WiFiClient client;

// ThingSpeak Settings
const int channelID = 471353;
String writeAPIKey = "DYO86R21RJUNE5YU"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";
const int postingInterval = 5 * 20; // post data every 5 seconds

int ldr = A0;
int led = 16;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  pinMode(ldr,INPUT);
  pinMode(led,OUTPUT);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
int val=0;
void loop() {
  //LDR
  float readLDR = ledpin();

        float ldrVoltage = (readLDR * AREF_VOLTAGE )/ (1024-readLDR); 
        float ldrResistance = (REF_RESISTANCE * (AREF_VOLTAGE - ldrVoltage)) / ldrVoltage; 
        float ldrpower= (ldrVoltage*ldrVoltage)/ldrResistance;
        float A_ldr = 1000000.0;
00        float B_ldr = -1.13;
        float ldrLux = A_ldr * pow(ldrResistance, B_ldr);
        float R=500/ldrLux;
        float v=5*R/(R+3.3);
        float P;
        if(led==1)
        {
          P=(v*v/R)*pow(10,-9);
        }
        else
          P=(v*v/R);
  //Wifi Module
  if (client.connect(server, 80)) {
    
    // Measure Signal Strength (RSSI) of Wi-Fi connection
    long rssi = WiFi.RSSI();

    // Construct API request body
    String body = "field1=";
           //body += String(ldrpower);
           body+=floatToString(readLDR,4);
           //body+=String("3000");
           //body += String(rssi);
           Serial.print("Power: ");
          // Serial.println(P);
    
    Serial.print("RSSI: ");
    Serial.println(rssi); 

    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: ESP8266 (nothans)/1.0");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(body.length()));
    client.println("");
    client.print(body);

  }
  client.stop();

  // wait and then post again
  delay(postingInterval);
}
float ledpin(){
  val = analogRead(ldr);
 if(val>=300)
 {
  digitalWrite(led,HIGH);
 }
 else
 {
   digitalWrite(led,LOW);
 }
Serial.println(val);
return val;
}
String floatToString(double number, uint8_t digits) 
{ 
  String resultString = "";
  // Handle negative numbers
  if (number < 0.0)
  {
    resultString += "-";
    number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  resultString += int_part;

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    resultString += "."; 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    resultString += toPrint;
    remainder -= toPrint; 
  } 
  return resultString;
}
