// RX parameter
#define RC_CHANS_STD               5
#define RC_CHANS_PPM               12
#define PCINT_PIN_COUNT            5
//PORTD : bit 0 and 1 is for TX/RX, shouldn't modify
#define PCINT_RX_BITS              (1<<2),(1<<4),(1<<5),(1<<6),(1<<7) //Activate pin 2 / 4 / 5 / 6 / 7
#define RX_PIN_ORDER               THROTTLEPIN, GIMBALROLLPIN, GIMBALPITCHPIN, YAWPIN, FMODEPIN
#define PCINT_RX_PORT              PORTD  //D Port (num. pin from 0 to 7) : Value 0 => Input / Value 1 => Output
#define PCINT_RX_MASK              PCMSK2 //Enable watch for pin value change
#define PCIR_PORT_BIT              (1<<2) //PCICR |= (1<<PCIE#), PCIE # is 2 for PORTD (D0-D7), this value enbale interrupt PCMSK2
#define RX_PC_INTERRUPT            PCINT2_vect //D0-D7 = PCINT 16-23 = PCIR2 = PD = PCIE2 = pcmsk2
#define RX_PCINT_PIN_PORT          PIND
#define LEDPIN_PINMODE             pinMode (13, OUTPUT);
#define LEDPIN_TOGGLE              PINB |= 1<<5;     //switch LEDPIN state (digital PIN 13)
#define LEDPIN_OFF                 PORTB &= ~(1<<5);
#define LEDPIN_ON                  PORTB |= (1<<5);

//RX PIN assignment inside the port //for PORTD
#define THROTTLEPIN                2 //For PPM Sum, this pin is used for all channels
#define GIMBALROLLPIN              4
#define GIMBALPITCHPIN             5
#define YAWPIN                     6
#define FMODEPIN                   7 //Naza mode

#define ENABLED                    1
#define DISABLED                   0

#define TRUE                       1
#define FALSE                      0

enum STD_ORDERED_CHANNEL_COMPUTE {
  THROTTLE_STD,
  GIMBALROLL_STD, 
  GIMBALPITCH_STD, 
  YAW_STD, 
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
//Real limite is 1180 - 1195
#define FLIGHTMODE_ACRO_PMW_LOW    1000
#define FLIGHTMODE_ACRO_PMW_HIG    1200
#define FLIGHTMODE_ACRO_MODE       1

//Real limite is 1510 - 1525
#define FLIGHTMODE_ATTI_PMW_LOW    1490
#define FLIGHTMODE_ATTI_PMW_HIG    1530
#define FLIGHTMODE_ATTI_MODE       2 

//Real limite is 1850 - 1865
#define FLIGHTMODE_GPS_PMW_LOW     1850
#define FLIGHTMODE_GPS_PMW_HIG     2000
#define FLIGHTMODE_GPS_MODE        5 

#define FLIGHTMODE_FAIL_MODE       6
