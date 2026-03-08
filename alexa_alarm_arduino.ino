#include "arduino_secrets.h"
/*
  Project: Alexa_Alarm
  Author: txomin
  Email: df.antonacci@gmail.com
  Date: 08/03/2026
  Revision: 2.1
  License: Public Domain
*/

#include "thingProperties.h"

#define DEBUG 0								  // Set to 1 to enable debug
#define PINarm 2
#define PINhome 3
#define PINpanic 4
#define PINdisarm 5

// Timing constants
#define CONNECTION_STABILIZATION_DELAY 20000  // Milliseconds
#define ACTION_COOLDOWN 3000                  // Milliseconds
#define TRIGGER_DURATION 1000                 // Milliseconds

// System state
int activeTriggerPin = -1;                    // Stores the currently active pin that triggered an action, -1 means no trigger is currently active.
unsigned long triggerStartTime = 0;           // Records the timestamp (in milliseconds) when a trigger was activated, used with TRIGGER_DURATION constant.
bool connectionEstablished = false;           // Indicates connection to the cloud, updated in updateConnectionStatus.
unsigned long connectionTimestamp = 0;        // Stores the time when the connection was established, used with CONNECTION_STABILIZATION_DELAY constant.
unsigned long lastActionTime = 0;			  // Stores the timestamp of the last action performed, used with ACTION_COOLDOWN constant.
bool connectionStable = false;				  // Becomes true only after the connection has remained established for CONNECTION_STABILIZATION_DELAY constant.

void setup() {
  #if DEBUG
    Serial.begin(9600);
    delay(1500);
    Serial.println(F("\nSystem Initialized"));
  #endif

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  const int pins[] = {PINarm, PINhome, PINpanic, PINdisarm, LED_BUILTIN};
  for(int i = 0; i < 5; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
}

void loop() {
  ArduinoCloud.update();
  updateConnectionStatus();
  handleStatusLED();
  processTrigger();
}

// Cloud handlers
void onArmChange()    { if(arm)    { executeAction(PINarm, "ARM");    arm = false; } }
void onHomeChange()   { if(home)   { executeAction(PINhome, "HOME");   home = false; } }
void onDisarmChange() { if(disarm) { executeAction(PINdisarm, "DISARM"); disarm = false; } }
void onPanicChange()  { if(panic)  { executeAction(PINpanic, "PANIC");  panic = false; } }

// Sets connectionStable variable to true only after the connection has remained established for CONNECTION_STABILIZATION_DELAY constant.
void updateConnectionStatus() {
  bool currentStatus = ArduinoCloud.connected();
  
  if (currentStatus != connectionEstablished) {
    #if DEBUG
      Serial.print(F("[CONN] "));
      Serial.println(currentStatus ? "Connected" : "Disconnected");
    #endif
    if (currentStatus) {
      connectionTimestamp = millis();
      connectionStable = false; // Reset stabilization flag
    }
  }
  
  if (currentStatus && !connectionStable && millis() - connectionTimestamp >= CONNECTION_STABILIZATION_DELAY) {
    connectionStable = true;
    #if DEBUG
      Serial.println(F("[CONN] Connection stabilized"));
    #endif
  }
  
  connectionEstablished = currentStatus;
}

// Handles the bult-in LED. If the device is disconnected, the LED blinks.
// Uses conditional (ternary) operator condition ? value_if_true : value_if_false
void handleStatusLED() {
  digitalWrite(LED_BUILTIN, connectionEstablished ? 
    LOW : (millis() % 2000 < 1000));
}

// Ensures that a trigger remains active only for TRIGGER_DURATION milliseconds.
// Automatically resets activeTriggerPin when the time is up.
void processTrigger() {
  if(activeTriggerPin != -1 && 
     millis() - triggerStartTime >= TRIGGER_DURATION) {
    #if DEBUG
      Serial.print(F("[PIN] Released "));
      Serial.println(activeTriggerPin);
    #endif
    digitalWrite(activeTriggerPin, LOW);
    activeTriggerPin = -1;
  }
}

//  Triggers a specific action by activating a corresponding pin.
void executeAction(int pin, const char* action) {
  if(!isActionAllowed(action)) return;

  #if DEBUG
    Serial.print(F("[ACTION] "));
    Serial.println(action);
  #endif

  if(activeTriggerPin != -1) {
    #if DEBUG
      Serial.print(F("[PIN] Releasing previous "));
      Serial.println(activeTriggerPin);
    #endif
    digitalWrite(activeTriggerPin, LOW);
  }

  digitalWrite(pin, HIGH);
  activeTriggerPin = pin;
  triggerStartTime = millis();
  lastActionTime = millis();

  #if DEBUG
    Serial.print(F("[PIN] Activated "));
    Serial.println(pin);
  #endif
}

// Determines whether a specific action is permitted based on connectionStable and ACTION_COOLDOWN
bool isActionAllowed(const char* action) {
  if (!connectionStable) {
    #if DEBUG
      Serial.print(F("[DEBOUNCE] "));
      Serial.print(action);
      Serial.println(F(" blocked (waiting for connection stabilization)"));
    #endif
    return false;
  }
  
  if (millis() - lastActionTime < ACTION_COOLDOWN) {
    #if DEBUG
      Serial.print(F("[DEBOUNCE] "));
      Serial.print(action);
      Serial.print(F(" blocked ("));
      Serial.print(millis() - lastActionTime);
      Serial.println(F("ms since last action)"));
    #endif
    return false;
  }
  return true;
}
