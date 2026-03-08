#include "arduino_secrets.h"
/*
  Project: Alexa_Alarm
  Author: txomin
  Email: df.antonacci@gmail.com
  Date: 17/08/2022
  Revision: 1.0
  License: Public Domain
*/

#include "thingProperties.h"

#define DEBUG 0
#define PINalarm 2
#define PINpartial 4
#define PINpanic 6
#define PINdisable 8
#define connection_establishedDelay 30 //Delay in seconds after a new conncetion is established before running Alexa Methods
boolean connection_establishedState = false;
unsigned long connection_establishedMillis = 0;
#if DEBUG
  int whois = 0; //Who is the culprit?
#endif

void setup() {
  #if DEBUG
    Serial.begin(9600);  //Initialize serial
    delay(1500);  //Wait for port to open
  #endif
  initProperties();   //Defined in thingProperties.h
  ArduinoCloud.begin(ArduinoIoTPreferredConnection); //Connect to Arduino IoT Cloud
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  pinMode(PINalarm, OUTPUT);
  pinMode(PINpartial, OUTPUT);
  pinMode(PINpanic, OUTPUT);
  pinMode(PINdisable, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  ArduinoCloud.update();
  switch (ArduinoCloud.connected()) {
    case false:
      connection_establishedState = false;
      #if DEBUG
        Serial.println("Connecting...");
      #endif
      break;
    case true:
      if (connection_establishedState == 0) {
        connection_establishedState = true;
        connection_establishedMillis = millis();
      }
      digitalWrite(LED_BUILTIN, (millis() % 5000) < 1000);
      #if DEBUG
          if (millis() - connection_establishedMillis <= connection_establishedDelay * 1000) {
            Serial.print("New connection detected, waiting ");
            Serial.print(connection_establishedDelay);
            Serial.println(" secs...");
          }
      #endif
      break;
  }    
}

void onAlarmChange()  {
  if (millis() - connection_establishedMillis <= connection_establishedDelay * 1000) {
    #if DEBUG
        Serial.print("New connection detected, waiting ");
        Serial.print(connection_establishedDelay);
        Serial.println(" secs...");
    #endif
    return;
  }
  switch (alarm) {
    case true:
      #if DEBUG
        Serial.println("Sending command: ALARM");
      #endif
      digitalWrite(PINalarm, HIGH);
      delay(500);
      digitalWrite(PINalarm, LOW);
      alarm = false; //Not really needed, just for aesthetics inside the Alexa app
      break;
    case false:
      #if DEBUG
        whois = 1;
      #endif
      DisableAlarm();
      break;
  }
}

void onPartialChange()  {
  if (millis() - connection_establishedMillis <= connection_establishedDelay * 1000) {
    #if DEBUG
        Serial.print("New connection detected, waiting ");
        Serial.print(connection_establishedDelay);
        Serial.println(" secs...");
    #endif
    return;
  }
  switch (partial) {
    case true:
      #if DEBUG
        Serial.println("Sending command: PARTIAL");
      #endif
      digitalWrite(PINpartial, HIGH);
      delay(500);
      digitalWrite(PINpartial, LOW);
      partial = false; //Not really needed, just for aesthetics inside the Alexa app
      break;
    case false:
      #if DEBUG
        whois = 2;
      #endif
      DisableAlarm();
      break;
  }
}

void onPanicChange()  {
 if (millis() - connection_establishedMillis <= connection_establishedDelay * 1000) {
    #if DEBUG
        Serial.print("New connection detected, waiting ");
        Serial.print(connection_establishedDelay);
        Serial.println(" secs...");
    #endif
    return;
  }
  switch (panic) {
    case true:
      #if DEBUG
        Serial.println("Sending command: PANIC");
      #endif
      digitalWrite(PINpanic, HIGH);
      delay(500);
      digitalWrite(PINpanic, LOW);
      panic = false; //Not really needed, just for aesthetics inside the Alexa app
      break;
    case false:
      #if DEBUG
        whois = 3;
      #endif
      DisableAlarm();
      break;
  }
}

void DisableAlarm()  {
  #if DEBUG
    Serial.print("Sending command: DISABLE ");
    Serial.println(whois);
  #endif
  digitalWrite(PINdisable, HIGH);
  delay(500);
  digitalWrite(PINdisable, LOW);
  delay(1000);
  digitalWrite(PINdisable, HIGH);
  delay(500);
  digitalWrite(PINdisable, LOW);
}