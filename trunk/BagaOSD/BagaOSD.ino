#include <inttypes.h>
#include "vars.h"
#include "def.h"
#include "config.h"
#include <RSSIFilter.h> 
#include <SendOnlySoftwareSerial.h> //Same as SoftwareSerial, but send only data. Used to send data to MinimOSD

//#define DEBUG_SENSOR			//uncomment to enable debugging output to console and stop MAVLink messages from being transmitted
//#define DEBUG_LOOP
//#define DEBUG_GPS
#include "../GCS_MAVLink/include/mavlink/v1.0/mavlink_types.h"
#include "RunningUintAverage.h" //For analog sensor

#ifndef __AVR_ATmega328P__
#error Only ATmega328P board is supported
#endif

#if !defined(THROTTLE_PWM_MIN) || !defined(THROTTLE_PWM_MAX) || !(THROTTLE_PWM_MAX>THROTTLE_PWM_MIN)
#error Throttle not defined properly
#endif


//D8-D13 OK not used by PCINT2_vect, D0-D7 KO, used by PCINT2_vect (RX.ino) and Serial
//BagaSoftwareSerial gps(8, 9); //Read only GPS Data (no TX) : Use RX pin (8) to read, free pin for TX (9) not used
SendOnlySoftwareSerial minimosd(8);

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS
mavlink_system_t mavlink_system;
static inline void comm_send_ch(mavlink_channel_t chan, uint8_t ch)
{
    if (chan == MAVLINK_COMM_0)
    {
#if !defined(DEBUG_SENSOR) && !defined(DEBUG_LOOP) && !defined(DEBUG_GPS)
        minimosd.write(ch);
#endif
    }
}
#include "../GCS_MAVLink/include/mavlink/v1.0/common/mavlink.h"

static int16_t rcDataSTD[RC_CHANS_STD];    // interval [1000;2000]
static int16_t rcDataPPM[RC_CHANS_PPM];    // interval [1000;2000]

float VFinal = 0;
float IFinal = 0;

RSSIFilter rssiFilter(RSSI_RC_FILTER, RSSI_RSSI_MAX, RSSI_SCALE_MIN, RSSI_SCALE_MAX); //PWM rssi
RunningUintAverage VFinalUint(VOLTAGE_BUFFER); //Average voltage
RunningUintAverage IFinalUint(CURRENT_BUFFER); //Average current

void setup() {
  configureReceiver();
  minimosd.begin(57600); //For minimosd communication use standard Serial function
  
  //Different speed if decoding Naza Protocol or reading raw uBlox Data
  #if defined(DECODE_NAZA_GPS) && DECODE_NAZA_GPS == TRUE
    Serial.begin(115200);            //Decode Naza data at 115200 bauds
  #else
    Serial.begin(57600);            //Raw reads (57600 bauds) GPS on RX, transmits output on TX
  #endif

  
  #if defined(RSSI_USE_PWM) && RSSI_USE_PWM == ENABLED 
    pinMode(RSSI_PIN_PWM,INPUT);      //RSSI PWM pin
    digitalWrite(RSSI_PIN_PWM,HIGH);  //internal pullup enable
    rssiFilter.clear();
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

    #if defined(DEBUG_GPS)
    unsigned long currtime_gps=millis();
    static unsigned long last_sent_current_time_gps=0;

    if(currtime_gps - last_sent_current_time_gps > 500){
      	last_sent_current_time_gps=currtime_gps;
        Serial.print("Lat: "); Serial.print(lat, 7);
        Serial.print(", Lon: "); Serial.print(lon, 7);
        Serial.print(", Alt: "); Serial.print(alt, 7);
        Serial.print(", Fix: "); Serial.print(gpsFix);
        Serial.print(", Sat: "); Serial.println(numsats);
        Serial.print("Heading: "); Serial.println(heading, 2);
    }
    #endif
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
       Serial.print(";");
        #if defined(RSSI_USE_PWM ) 
          Serial.print("PITCH PPM : ");
          Serial.print(rcDataPPM[PITCH_PPM]);
          Serial.print(";");
        #endif
        Serial.print("THROTTLE : ");
        Serial.print(rcDataPPM[THROTTLE_STD]);
        Serial.print(";");
        Serial.print("FMODE : ");
        Serial.print(rcDataSTD[FMODE_STD]);
        Serial.print(";");
        Serial.print("FMODE PPM: ");
        Serial.print(rcDataSTD[FMODE_PPM]);
        Serial.print(";");
        Serial.print("AUX CMD : ");
        Serial.print(rcDataSTD[AUX_STD]);
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
