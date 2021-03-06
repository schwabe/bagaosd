#define ToDeg(x) ( x * RAD_TO_DEG)	

// RX parameter
#define RC_CHANS_STD               5
#define RC_CHANS_PPM               12
//PORTD : bit 0 and 1 is for TX/RX, shouldn't modify

//RX PIN assignment inside the port //for PORTD
#define THROTTLEPIN                2 //For PPM Sum, this pin is used for all channels
#define GIMBALROLLPIN              4
#define GIMBALPITCHPIN             5
#define AUXPIN                     6
#define FMODEPIN                   7 //Naza mode


#if !defined(RC_PPM_MODE)
  //Activate pin 2 / 4 / 5 / 6 / 7 (Throttle / Gimbal Roll / Gimbal Pitch / Panel Switch /Flight mode)
  #define PCINT_RX_BITS              (1<<THROTTLEPIN),(1<<GIMBALROLLPIN),(1<<GIMBALPITCHPIN),(1<<AUXPIN),(1<<FMODEPIN) 
  #define PCINT_PIN_COUNT            5
#else
  //Activate pin 2 / 4 / 5 (PPM / Gimbal Roll / Gimbal Pitch)
  #define PCINT_RX_BITS              (1<<THROTTLEPIN),(1<<GIMBALROLLPIN),(1<<GIMBALPITCHPIN)               
  #define PCINT_PIN_COUNT            3
#endif

#define PCINT_RX_PORT              PORTD  //D Port (num. pin from 0 to 7) : Value 0 => Input / Value 1 => Output
#define PCINT_RX_MASK              PCMSK2 //Enable watch for pin value change
#define PCIR_PORT_BIT              (1<<2) //PCICR |= (1<<PCIE#), PCIE # is 2 for PORTD (D0-D7), this value enbale interrupt PCMSK2
#define RX_PC_INTERRUPT            PCINT2_vect //D0-D7 = PCINT 16-23 = PCIR2 = PD = PCIE2 = pcmsk2
#define RX_PCINT_PIN_PORT          PIND
#define LEDPIN_PINMODE             pinMode (13, OUTPUT);
#define LEDPIN_TOGGLE              PINB |= 1<<5;     //switch LEDPIN state (digital PIN 13)
#define LEDPIN_OFF                 PORTB &= ~(1<<5);
#define LEDPIN_ON                  PORTB |= (1<<5);

#define ENABLED                    1
#define DISABLED                   0

#define MIXTE                      2
#define TRUE                       1
#define FALSE                      0

enum STD_ORDERED_CHANNEL_COMPUTE {
  THROTTLE_STD,
  GIMBALROLL_STD, 
  GIMBALPITCH_STD, 
  AUX_STD, 
  FMODE_STD
};

enum PPM_ORDERED_CHANNEL_COMPUTE {
  ROLL_PPM,
  PITCH_PPM,
  YAW_PPM,
  THROTTLE_PPM,
  X1_PPM,
  X2_PPM,
  FMODE_PPM,
  AUX4_PPM
};

//FAILSAFE LIMIT
//Real limite is 1180 - 1195 (1187)
#define FLIGHTMODE_ACRO_PMW_LOW    1000
#define FLIGHTMODE_ACRO_PMW_HIG    1250//1200
#define FLIGHTMODE_ACRO_MODE       1

//Real limite is 1510 - 1525 (1524)
#define FLIGHTMODE_ATTI_PMW_LOW    1450//1490
#define FLIGHTMODE_ATTI_PMW_HIG    1600//1530
#define FLIGHTMODE_ATTI_MODE       2 

//Real limite is 1850 - 1865 (1856)
#define FLIGHTMODE_GPS_PMW_LOW     1800//1850
#define FLIGHTMODE_GPS_PMW_HIG     2000
#define FLIGHTMODE_GPS_MODE        5 

//1698
#define FLIGHTMODE_FAIL_MODE       6


//Analog measure
#define RSSI_PIN_ANALOG_POS  0
#define VOLT_PIN_ANALOG_POS  1
#define CURR_PIN_ANALOG_POS  2

#define NUMBER_ANALOG_PIN    3
#define ANALOG_DEF_VAL       0,0,0   
#define NO_ANALOG_PIN        255
#define ANALOG_PIN_DEF_VAL   NO_ANALOG_PIN,NO_ANALOG_PIN,NO_ANALOG_PIN

//Time in milli to display battery size, before take off
#define BATTERY_DISPLAY_FTIME 1000

//Internal buffer
#define VOLTAGE_BUFFER             8   //Internal buffer to smooth voltage value
#define CURRENT_BUFFER             8   //Internal buffer to smooth current value
