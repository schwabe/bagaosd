#include <inttypes.h>
// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "wiring.h"
#endif
#include "vars.h"
#include "def.h"
#include "config.h"

//#define DEBUG_SENSOR			//uncomment to enable debugging output to console and stop MAVLink messages from being transmitted
//#define DEBUG_LOOP

#include "../GCS_MAVLink/include/mavlink/v1.0/mavlink_types.h"
#include "RunningUintAverage.h" //For anolog sensor and PWM RSSI

#ifndef __AVR_ATmega328P__
#error Only ATmega328P board is supported
#endif

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS
mavlink_system_t mavlink_system;
static inline void comm_send_ch(mavlink_channel_t chan, uint8_t ch)
{
    if (chan == MAVLINK_COMM_0)
    {
#if !defined(DEBUG_SENSOR) && !defined(DEBUG_LOOP)
        Serial.write(ch);
#endif
    }
}
#include "../GCS_MAVLink/include/mavlink/v1.0/common/mavlink.h"


static int16_t rcDataSTD[RC_CHANS_STD];    // interval [1000;2000]
static int16_t rcDataPPM[RC_CHANS_PPM];    // interval [1000;2000]
boolean alreadyConnected = false; 

float VFinal = 0;
float IFinal = 0;

RunningUintAverage rssi_percent(RSSI_PWM_BUFFER); //PWM rssi
RunningUintAverage VFinalUint(VOLTAGE_BUFFER); //Average voltage
RunningUintAverage IFinalUint(CURRENT_BUFFER); //Average current

void setup() {
  configureReceiver();

  Serial.begin(57600);		//reads GPS on RX, transmits output on TX
  
  #if RSSI_USE_PWM == ENABLED
    pinMode(RSSI_PIN_PWM,INPUT);      //RSSI PWM pin
    digitalWrite(RSSI_PIN_PWM,HIGH);  //internal pullup enable
    rssi_percent.clear();
  #endif

  VFinalUint.clear();
  IFinalUint.clear();

  LEDPIN_PINMODE
  delay(1000);
}

void computeData() {
    decode_gps();        //Compute GPS informations
    analyseRC();         //Compute radio channel
    checkRSSI();         //Compute RSSI
    checkFlightMode();   //Compute Flight Mode
    checkBattVolt();     //Compute Voltage and Current
    checkThrottle();     //Compute thottle percent
    
    #if !defined(ESTIMATE_BATTERY_REMAINING)
      unsigned long currtime=millis();
      static unsigned long lastsample=0;
      if(currtime-lastsample>50){
        updateCurrent(currtime-lastsample); //Update current consumption, calculate battery remaining
        lastsample = currtime;
      }
    #endif
    
    sendMavlinkMessages();
}

void loop(){
    computeData();   
    
    #if defined(DEBUG_LOOP)
    unsigned long currtime=millis();
    static unsigned long last_sent_current_time=0;
    if(currtime - last_sent_current_time > 500){
      	last_sent_current_time=currtime;
      
        Serial.print("GIMBALROLL : ");
        Serial.print(rcDataSTD[GIMBALROLL_STD]);
        Serial.print(";");
        Serial.print("GIMBALPITCH : ");
        Serial.print(rcDataSTD[GIMBALPITCH_STD]);
        #if defined(RSSI_USE_PWM ) 
          Serial.print(";");
          Serial.print("PITCH PPM : ");
          Serial.print(rcDataPPM[PITCH_PPM]);
        #endif
        Serial.print("THROTTLE : ");
        Serial.print(rcDataSTD[THROTTLE_STD]);
        Serial.print(";");
        Serial.print("FMODE : ");
        Serial.print(rcDataSTD[FMODE_STD]);
        Serial.print(";");
        Serial.print("FMODE PPM: ");
        Serial.print(rcDataSTD[FMODE_PPM]);
        Serial.print(";");
        Serial.print("YAW : ");
        Serial.print(rcDataSTD[YAW_STD]);
        Serial.println("");
        Serial.print("Voltage : ");
        Serial.print(long(VFinal*1000.0));
        Serial.print(";");
        Serial.print("Throttle : ");
        Serial.print(throttlepercent);
        Serial.print(";");
        Serial.print("Flight : ");
        Serial.print(flightmode);
        /*Serial.print(";");
        Serial.print("Last Flight : ");
        Serial.print(last_flightmode);
        Serial.print(";");
        Serial.print("total_timeFailSafe : ");
        Serial.print(total_timeFailSafe);
        Serial.print(";");
        Serial.print("total_timeNoFailSafe : ");
        Serial.print(total_timeNoFailSafe);*/
        #if ESTIMATE_BATTERY_REMAINING == ENABLED
          Serial.println(" ");
	  Serial.print("Estimated pack remaining (%): ");
	  Serial.println(estimatepower());
        #endif
        Serial.print(" RSSI : ");
        Serial.println(receiver_rssi);
    }
    #endif
    
}
