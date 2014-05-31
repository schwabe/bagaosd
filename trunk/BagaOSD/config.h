//Send FrSky telemetry data  
#define FRSKY_PROTOCOL            //Send telemetry data to FrSky radio
//#define FRSKY_DMS               //DMS (48°N52'13.782) or NMEA (48°52.1378N) - Default NMEA
//#define FRSKY_IMPERIALS         //Default Metrics
//#define FRSKY_FLD02             //Uncomment if FLD_02 is used (leave commented for TARANIS)

//RSSI Analog config
#define RSSI_PIN_ANALOG            A2   //Analog PIN for RSSI : A0, A1, A2, A3 (default A2)
#define RSSI_MIN_VAL               350  //RSSI min displayed value. This value can either represent a RSSI min or max signal, it doesn't matter.
#define RSSI_MAX_VAL               650//670  //RSSI max displayed value. This value can either represent a RSSI min or max signal, it doesn't matter.
#define RSSI_VAL_LOW               RSSI_MIN_VAL //This value should represent the value when the signal is the lowest (RSSI_MIN_VAL or RSSI_MAX_VAL)
//#define RSSI_RAW_VAL               //Uncomment if you want to use raw RSSI value, instead of percent computed value

//Radio receiver information
#define RC_PPM_MODE                //Uncomment to decode PPM Sum with arduino pin 2. This can be commented to use standard RX configuration instead of PPM Sum configuration.
#define SERIAL_SUM_PPM             ROLL_PPM,PITCH_PPM,THROTTLE_PPM,YAW_PPM,X1_PPM,X2_PPM,FMODE_PPM,AUX4_PPM //Order of PPM channels

//Sensor config
#define INTERNAL_VOLTAGE_REF       5160  //Internal voltage can vary, so to get the same measured value, we should compare to a fixed reference. This can be commented to be disabled.
//#define VOLTAGE_THROTTLE_COMP      0.14 //0 : no compensation. Used if sensor voltage drop too much when there's some throttle. This can be commented to be disabled.

//Voltage sensor
#define VOLTAGE_PIN                A0    //Voltage sensor pin
#define VOLTAGE_FACTOR             12.87 //Rise this value to lower voltage output value   

//Current sensor
#define CURRENT_PIN                A3    //Current sensor pin
#define CURRENT_FACTOR             14.9 //2.05  //1.85  //Rise this value to lower current output value   

//#define ESTIMATE_BATTERY_REMAINING  ENABLED   //Commented to use a current sensor, current sensor is not used if uncommented.
#define LIPO_CELL                  4     //Number of cell, for estimate capacity (ESTIMATE_BATTERY_REMAINING)
#define LIPO_CAPACITY_MAH          8000  //LIPO capacity in mah
#define LIPO_CAPACITY_MAH_MULTI    LIPO_CAPACITY_MAH,10000,5000  //LIPO capacity in mah selector
 
//GPS Handler
#define DECODE_NAZA_GPS            //Uncomment to decode Naza GPS Protocol - Comment and solder a wire on Naza GPS to decode RAW uBlox Message
//#define NAZA_COMPASS_TILT_COMP     MIXTE  //TRUE : Experimental Tilt compensate compass information from Naza GPS, FALSE no tilt compensation, use COG instead of heading. MIXTE : tilt compensate if not moving, COG for speed > 2m/s 

//Throttle
#define THROTTLE_PWM_MIN           1000
#define THROTTLE_PWM_MAX           2000

//Artificial horizon
#define PITCH_GAIN                 4.3 //Set Pitch Naza gain to 20 in Naza GUI
#define PITCH_LEVEL                1500
//#define PITCH_INVERT               //Uncomment if pitch output is reversed (if horizon is not moving up when copter has nose down)

#define ROLL_GAIN                  4.3 //Set Roll Naza gain to 20 in Naza GUI 
#define ROLL_LEVEL                 1500
//#define ROLL_INVERT                //Uncomment if roll output is reversed (if horizon (/) is not the opposite (\) of the copter position)

#define HOME_SET_PRECISION         0.2 //Precision to wait for 10s before setting altitude home (don't set lower than 0.1)
#define HOME_SET_AUTO_TIMEOUT      90 //Home will be automatically set after 90s
/**
Standard receiver configuration
Pin 2           :   Throttle
Pin 4           :   Gimbal roll for horizon display
Pin 5           :   Gimbal pitch for horizon display
Pin 6           :   AUX (for pannel switch in MinimOSD and Battery capacity change before flight)
Pin 7           :   FMode (Flight mode : Acro / Atti / Gps / Failsafe) - Flight Mode


PPM Sum receiver configuration
Pin 2           :   PPM Sum signal from receiver
Pin 4           :   Gimbal roll for horizon display  (it's not a PPM Sum signal)
Pin 5           :   Gimbal pitch for horizon display (it's not a PPM Sum signal)

RSSI Configuration 
Pin A2          :   Analog RSSI (#define RSSI_PIN_ANALOG)

Sensor
Voltage sensor  :   A0 (#define VOLTAGE_PIN)
Current sensor  :   A3 (#define CURRENT_PIN)
GPS             :   RX

**/

