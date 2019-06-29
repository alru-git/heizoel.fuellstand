/**
 * 
 * DESCRIPTION
 * This sketch provides an example how to implement a distance sensor using HC-SR04 
 * http://www.mysensors.org/build/distance
 * 
 * Messung des Ölpegels inkl. der Spannungsversorgung
 * Ver.2.0 wird für das Netzteil mit der Leerlaufspannung 12,7V benutzt
 * Ver.2.1 ("Heizoel_Distance") wurde die Sleep_Time angepasst
 * Die Spannungsmessung wurde aber beibehalten
 * Ver 2.2 auf neuen Kanal eigestellt
 * Ver 2.3 mit Node ID
 * Ver 3.0 ("Heizoel.Fuellstand") mit werden die Werte von lastDist und oldBatteryPcnt nicht mehr ausgewertet,
 *  sondern wg. der "alive" Überprüfung IMMER alle 6 Std. gesendet
 * Doku siehe mysensors_doku.odt
 * Ver 3.1
 *  heartbeat eingefügt
 *  Es wird nur noch der %-Wert der Batteriespg. übertragen
 * Ver 3.2
 *  Umstieg von heartbeat auf smartSleep mit Wait_Duration, da das Versenden öfter Probleme bereitet hat
 *  Sleep_Time auf 3h
 * Ver 3.3
 *  Umstieg von smartSleep auf heartbeat, da das Versenden öfter Probleme bereitet hat
 *  Sleep_Time auf 1h
 *
 */

// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24

// MSG1 (Frequenz 2496 MHz)
#define MY_RF24_CHANNEL 96

// Optional: Define Node ID
#define MY_NODE_ID 101
// Node 0: MSG1 oder MSG2
// #define MY_PARENT_NODE_ID 0
// #define MY_PARENT_NODE_IS_STATIC

#include <SPI.h>
#include <MySensors.h>  
#include <NewPing.h>

// Battery-Kontrolle
  int BATTERY_SENSE_PIN = A0;
// Ende 

#define CHILD_ID 1
#define TRIGGER_PIN  6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     5  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
unsigned long SLEEP_TIME = 3600000; // Sleep time between reads (in milliseconds: entspricht 1h)

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
MyMessage msg(CHILD_ID, V_DISTANCE);
bool metric = true;

void setup()  
{ 
  metric = getControllerConfig().isMetric;

  // Battery-Kontrolle ...
    #if defined(__AVR_ATmega2560__)
      analogReference(INTERNAL1V1);
    #else
      analogReference(INTERNAL);
    #endif
  // ... Ende 
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Heizoel.Fuellstand", "3.3");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID, S_DISTANCE);
}

void loop()      
{     
  //Aufwachen und Update vom Reading heartbeat und state
  wait(10000);
  sendHeartbeat();
    
  int dist = metric?sonar.ping_cm():sonar.ping_in();
  Serial.print("Ping: ");
  Serial.print(dist); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.println(metric?" cm":" in");

  send(msg.set(dist));
  
  // Battery-Kontrolle ...
    int sensorValue = analogRead(BATTERY_SENSE_PIN);
    #ifdef MY_DEBUG
      Serial.println(sensorValue);
    #endif
  
    int batteryPcnt = sensorValue / 10 * 1.0;

    #ifdef MY_DEBUG
      float batteryV  = sensorValue * 0.003363075;
      Serial.print("Battery Voltage: ");
      Serial.print(batteryV);
      Serial.println(" V");
  
      Serial.print("Battery percent: ");
      Serial.print(batteryPcnt);
      Serial.println(" %");
    #endif

   sendBatteryLevel(batteryPcnt);
   
  // ... Ende

  sleep(SLEEP_TIME);
}


