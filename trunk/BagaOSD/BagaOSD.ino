#include <inttypes.h>
#include "def.h"
#include "config.h"
#include "vars.h"
#include <SendOnlySoftwareSerial.h> //Same as SoftwareSerial, but send only data. Used to send data to MinimOSD

//#define DEBUG_SENSOR			//uncomment to enable debugging output to console and stop MAVLink messages from being transmitted
//#define DEBUG_LOOP
//#define DEBUG_FRSKY
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

volatile int8_t cppm_loop=0;

float VFinal = 0;
float IFinal = 0;

RunningUintAverage VFinalUint(VOLTAGE_BUFFER); //Average voltage
RunningUintAverage IFinalUint(CURRENT_BUFFER); //Average current

void setup() {                     
  configureReceiver();   
  minimosd.begin(57600);            //For minimosd communication use standard Serial function
  
  //Different speed if decoding Naza Protocol or reading raw uBlox Data
  #if defined(DECODE_NAZA_GPS) 
    Serial.begin(115200);           //Decode Naza data at 115200 bauds
  #else
    Serial.begin(57600);            //Raw reads (57600 bauds) GPS on RX, transmits output on TX
  #endif

  #if defined(FRSKY_PROTOCOL) 
    sendTelemetry.begin(9600);
  #endif
  
  computeIntervalVCC();

  LEDPIN_PINMODE

  delay(1000);
}

void computeData() {
    unsigned long mcurrtime = micros();
    decode_gps();          //Compute GPS informations
    
    static unsigned long lastrc=0;
    if(mcurrtime>lastrc){  // 500Hz
      analyseRC();         //Compute radio channel
      lastrc = mcurrtime + 2000;
    }
    
    checkThrottle();     //Compute thottle percent
    checkFlightMode();   //Compute Flight Mode
      
    //Should be more than 27, because PPM Sum can be 27ms long
    static unsigned long lastcomputetime;
    if( mcurrtime > lastcomputetime ) {
        handleStableFlightMode();
        lastcomputetime = mcurrtime + 30000;
    }
    
    #if defined(RSSI_PIN_ANALOG) || defined(VOLTAGE_PIN) || defined(CURRENT_PIN)
      static unsigned long lastsampleanalog=0;
      static byte analog_selector=0;
      if(mcurrtime-lastsampleanalog>10000){ //100Hz
        switch (analog_selector) {
          #if defined(RSSI_PIN_ANALOG)
          case 0:
            checkRSSI(analogRead(RSSI_PIN_ANALOG));
          break;
          #endif
          #if defined(VOLTAGE_PIN)
          case 1:
            checkBattVolt(analogRead(VOLTAGE_PIN));
          break;
          #endif
          #if defined(CURRENT_PIN)
          case 2:
            checkBattCurrent(analogRead(CURRENT_PIN));
          break;
          #endif
        }
  
        lastsampleanalog = mcurrtime;
        analog_selector++;
        if( analog_selector > 2) analog_selector=0;
      }
    #endif
    
    #if !defined(ESTIMATE_BATTERY_REMAINING)
      static unsigned long lastsample=0;
      if(mcurrtime-lastsample>20000){ //50Hz
        updateCurrent(mcurrtime-lastsample); //Update current consumption, calculate battery remaining
        lastsample = mcurrtime;
      }
    #endif
    
    sendMavlinkMessages();
    
    #if defined(FRSKY_PROTOCOL)
      update_FrSky();
    #endif
}


void loop(){
    unsigned long currtime = millis();
    computeData();   
    
    
    #if defined(DEBUG_GPS)
    static unsigned long last_sent_current_time_gps=0;

    if(currtime - last_sent_current_time_gps > 500){
      	last_sent_current_time_gps=currtime;
        Serial.print("Lat: "); Serial.print(lat, 7);
        Serial.print(", Lon: "); Serial.print(lon, 7);
        Serial.print(", Alt: "); Serial.print(alt_m, 7);
        Serial.print(", Fix: "); Serial.print(gpsFix);
        Serial.print(", Sat: "); Serial.println(numsats);
        Serial.print("Heading: "); Serial.println(heading_d, 2);
    }
    #endif
    #if defined(DEBUG_LOOP)
    static unsigned long last_sent_current_time=0;

    if(currtime - last_sent_current_time > 500){
      	last_sent_current_time=currtime;
      	#if defined(RC_PPM_MODE)
      	    Serial.print("PPM;");
      	    Serial.print("TH:");
            Serial.print(rcDataPPM[THROTTLE_PPM]);
            Serial.print(":FM:");
            Serial.print(rcDataPPM[FMODE_PPM]);
            Serial.print(":X1:");
            Serial.print(rcDataPPM[X1_PPM]);
            Serial.print(":X2:");
            Serial.print(rcDataPPM[X2_PPM]);
            Serial.print(":AUX4:");
            Serial.print(rcDataPPM[AUX4_PPM]);
            Serial.print(":CPPM:");
            Serial.print(cppm_loop);
            Serial.print(";");
        #else
            Serial.print("STD;");
            Serial.print("AUX:");
            Serial.print(rcDataSTD[AUX_STD]);
            Serial.print(";");
      	#endif
      	
      	//When PPM is used, THROTTLE / FMODE values are mapped to STD
      	Serial.print("COM;");
  	Serial.print("TH:");
        Serial.print(rcDataSTD[THROTTLE_STD]);
        Serial.print(":FM:");
        Serial.print(rcDataSTD[FMODE_STD]);     
      	
      	
      	//Common values
        Serial.print(":GR:");
        Serial.print(rcDataSTD[GIMBALROLL_STD]);
        Serial.print(":");
        Serial.print((long)ToDeg(roll_rad));
        Serial.print(":CP:");
        Serial.print(rcDataSTD[GIMBALPITCH_STD]);
        Serial.print(":");
        Serial.print((long)ToDeg(pitch_rad));
        Serial.print(":");
        Serial.println("");
        
        Serial.print("VOLT:");
        Serial.print(long(VFinal*1000.0));
        Serial.print(":TH:");
        Serial.print(throttlepercent);
        Serial.print(":FMODE:");
        Serial.print(flightmode);
        #if ESTIMATE_BATTERY_REMAINING == ENABLED
          Serial.println(" ");
	  Serial.print("Estimated pack remaining (%): ");
	  Serial.println(estimatepower());
        #endif
        Serial.print(":FTIME:");
        Serial.print(flight_time);
        Serial.print(":RSSI:");
        Serial.print(receiver_rssi);
        Serial.print(":BATT:");
        Serial.println(battery_capacity);
        Serial.println("-");
        
        #if defined(DEBUG_SENSOR)
        Serial.print("VOLT_LAST:");
	Serial.print(VFinal);
        Serial.print(":AVG:");
        Serial.print(VFinalUint.getAverage());
        Serial.print(":INT_VCC:");
        Serial.print(intervalVCC);
		
        #if ESTIMATE_BATTERY_REMAINING == ENABLED
	  Serial.print(":Estimated pack remaining (%): ");
	  Serial.println(estimatepower());
        #else
          Serial.print(":CURR_LAST:");
	  Serial.print(IFinal);
          Serial.print(":AVG:");
          Serial.print(IFinalUint.getAverage());
          Serial.print(":INT_VCC:");
          Serial.println(intervalVCC);
        #endif
        Serial.print("CONSO:");
	Serial.print(mahout);
        Serial.print("REMAINING:");
        Serial.println(capacity);
        Serial.println(" ");
      #endif
    }
    #endif
    
}
