/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK "thereisnospoon"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

const char *ssidSTA = "";
const char *passwordSTA = "";


int switchPins[] = {0,1,2,3,4,5,12,13,14, 16};
ESP8266WebServer server(80);

int delayTime = 0;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot()
{
  Serial.println("hit");
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void handleConfig()
{
  String ssid = server.arg("ssid");
  String pwd = server.arg("pwd");
  Serial.println(ssid);
  Serial.println(pwd);
  Serial.println(WiFi.begin(ssid, pwd));
    Serial.println(WiFi.localIP());

  server.send(200, "text/html", "config updated");
}

void handleFreq()
{
  String freq = server.arg("f");
  int fre = freq.toInt();
  analogWriteFreq(fre);
  server.send(200, "text/html", "freq updated");
}

void performSwitch(int switchPin, int state)
{
  if(state == 0 || state==1){
    digitalWrite(switchPin, state);
    Serial.println("digi");
    Serial.println(switchPin);
    delayTime = 0;
  }
  else
      { 
        // analogWrite(switchPin, state);  
        delayTime = state;
          Serial.println("analog");
      }
}

void handleControl()
{
  String sw = server.arg("sw");
  String st = server.arg("st");
//  int state;
//  if (st == "0") state = LOW;
//  else if (st == "1") state = HIGH;
//  else return;

  int state = st.toInt();
  int swtchNo = sw.toInt();
  int switchPin = switchPins[swtchNo];
  

  performSwitch(switchPin, state);
  server.send(200, "text/html", "control updated");
}

void initPins()
{
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(16, OUTPUT);
}
void setServerCallbacksAndStart()
{
  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/control", handleControl);
  server.on("/setfreq", handleFreq);
  server.begin();
  Serial.println("HTTP server started");

  Serial.println(WiFi.status() == WL_CONNECTED);
}
void startAPandSTA()
{
  Serial.print("Configuring access point...");
  // WiFi.disconnect(true)
  /* You can add the password parameter if you want the AP to be password protected. */
  WiFi.softAP(ssid);
  WiFi.begin(ssidSTA, passwordSTA); // both ssid and pwd are empty so that saved cred is used. the saved cred is saved when setting up config
   IPAddress myIP = WiFi.softAPIP();
   Serial.print("AP IP address: ");
   Serial.println(myIP);
     Serial.println("Router IP:");
    Serial.println(WiFi.localIP());
}
void handlePubSubMessages(){
  
}

void zcInterrupt(){
Serial.println("ZC");
}

int count=0;
int lastCount=0;

void ICACHE_RAM_ATTR handleInterrupt() { 
    count+=1;

    // Serial.println("interrupt");
return ; 
}

void setup()
{
  Serial.begin(115200);
  initPins();
  setServerCallbacksAndStart();
  startAPandSTA();
  analogWriteFreq(0.1);  
  // attachInterrupt(5, zcInterrupt, CHANGE);

  const int interruptPin = 5;
    pinMode(interruptPin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING); 
}



void turnOn(){
  if(delayTime>0){
   delayMicroseconds(delayTime);
  //  Serial.println(delayTime);
  digitalWrite(16,HIGH);
  delay(1);
  digitalWrite(16,LOW);
  }
}

void loop()
{
  server.handleClient();
  handlePubSubMessages();
// Serial.println("outside if "+count) ;  
// Serial.println("loop");
if(lastCount != count)
{
  lastCount = count;
// Serial.println(count) ;  
turnOn() ; 

}

//  delay(1000);
}
