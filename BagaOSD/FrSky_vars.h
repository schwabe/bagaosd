#if defined(FRSKY_PROTOCOL)
// FrSky modules addon

unsigned char outBuff[48];     // Array for payloads
unsigned char outBuffFixed[48];     // Array for payloads

boolean packetOpen;
byte payloadLen;
byte msCounter;

byte hour;
byte minutes;
byte second;

float fr_speed;
float fr_volt;
int fr_amp;
char fr_EW;
char fr_NS;
float fr_alt;
float fr_alt_msl;
float fr_temp;

unsigned char Frsky_Count_Order_Batt;
long Frsky_Batt_Volt_A; 
byte Batt_Cell_Detect=0;

unsigned long f_curMillis;
unsigned long f_preMillis;
int f_delMillis = 200;
// FrSky module addon - END

#endif
