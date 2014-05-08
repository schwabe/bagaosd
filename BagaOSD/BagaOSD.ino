#include <inttypes.h>
#include "def.h"
#include "vars.h"
#include "config.h"
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
SendOnlySoftwareSerial minimosd(8);

#if defined(FRSKY_PROTOCOL)
SendOnlySoftwareSerial sendTelemetry(9, true);
#endif

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

int16_t rcDataSTD[RC_CHANS_STD];    // interval [1000;2000]
int16_t rcDataPPM[RC_CHANS_PPM];    // interval [1000;2000]

float VFinal = 0;
float IFinal = 0;

RunningUintAverage VFinalUint(VOLTAGE_BUFFER); //Average voltage
RunningUintAverage IFinalUint(CURRENT_BUFFER); //Average current
RunningUintAverage RFinalUint(RSSI_BUFFER); //Average RSSI

void setup() {
  configureReceiver();
  minimosd.begin(57600); //For minimosd communication use standard Serial function
  
  //Different speed if decoding Naza Protocol or reading raw uBlox Data
  #if defined(DECODE_NAZA_GPS) && DECODE_NAZA_GPS == TRUE
    Serial.begin(115200);            //Decode Naza data at 115200 bauds
  #else
    Serial.begin(57600);            //Raw reads (57600 bauds) GPS on RX, transmits output on TX
  #endif

  #if defined(FRSKY_PROTOCOL)
    sendTelemetry.begin(9600);
  #endif
  
  init_analog();
  
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
    read_analog(millis());
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
    #if defined(FRSKY_PROTOCOL)
      update_FrSky();
    #endif
    
    #if defined(DEBUG_GPS)
    unsigned long currtime_gps=millis();
    static unsigned long last_sent_current_time_gps=0;

    if(currtime_gps - last_sent_current_time_gps > 500){
      	last_sent_current_time_gps=currtime_gps;
        Serial.print("Lat: "); Serial.print(lat, 7);
        Serial.print(", Lon: "); Serial.print(lon, 7);
        Serial.print(", Alt: "); Serial.print(alt_m, 7);
        Serial.print(", Fix: "); Serial.print(gpsFix);
        Serial.print(", Sat: "); Serial.println(numsats);
        Serial.print("Heading: "); Serial.println(heading_d, 2);
    }
    #endif
    #if defined(DEBUG_LOOP)
    unsigned long currtime=millis();
    static unsigned long last_sent_current_time=0;

    if(currtime - last_sent_current_time > 500){
      	last_sent_current_time=currtime;
      	#if RC_PPM_MODE == ENABLED 
      	    Serial.print("PPM;");
      	    Serial.print("THROTTLE : ");
            Serial.print(rcDataPPM[THROTTLE_PPM]);
            Serial.print(";");
            Serial.print("FMODE : ");
            Serial.print(rcDataPPM[FMODE_PPM]);
            Serial.print(";");
            Serial.print("X1_PPM : ");
            Serial.print(rcDataPPM[X1_PPM]);
            Serial.print(";");
            Serial.print("X2_PPM : ");
            Serial.print(rcDataPPM[X2_PPM]);
            Serial.print(";");
            Serial.print("AUX4_PPM : ");
            Serial.print(rcDataPPM[AUX4_PPM]);
            Serial.print(";");
        #else
            Serial.print("STD;");
            Serial.print("AUX CMD : ");
            Serial.print(rcDataSTD[AUX_STD]);
            Serial.print(";");
      	#endif
      	
      	//When PPM is used, THROTTLE / FMODE values are mapped to STD
      	Serial.print("COM;");
  	Serial.print("THROTTLE : ");
        Serial.print(rcDataSTD[THROTTLE_STD]);
        Serial.print(";");
        Serial.print("FMODE : ");
        Serial.print(rcDataSTD[FMODE_STD]);
        Serial.print(";");
        
      	
      	
      	//Common values
        Serial.print("GIMBALROLL : ");
        Serial.print(rcDataSTD[GIMBALROLL_STD]);
        Serial.print(":");
        Serial.print((long)ToDeg(roll_rad));
        Serial.print(":");
        Serial.print(";");
        Serial.print("GIMBALPITCH : ");
        Serial.print(rcDataSTD[GIMBALPITCH_STD]);
        Serial.print(":");
        Serial.print((long)ToDeg(pitch_rad));
        Serial.print(":");
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
