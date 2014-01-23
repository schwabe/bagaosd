float pitchgain=12.0;
float rollgain=7.6;

bool invertpitch=false;
bool invertroll=false;

//Flight mode
int flightmode=5;               //2=Alt Hold(Attitude), 1=Acrobatic, 6=Return to launch(Failsafe), 5=Loiter(GPS Attitude);

//Gimbal position 
float pitch_rad=10;
float roll_rad=10;
long levelpitch=0;
long levelroll=0;
long pitch=1500;
long roll=1500;

float capacity=0;
float mahout=0;

//Other sensors
int receiver_rssi=0;
int throttlepercent=0;


/*GPS variables*/
int gpsFix=1;
float lat=0;
float lon=0;
float alt_MSL=0;
long iTOW=0;
float alt=0;
short hdop=9999;
short vdop=9999;
float speed_3d=0;
float ground_speed=0;
float heading=90;
float climb=0;
char data_update_event=0;
short time_year=0;
int time_month=0;
int time_day=0;
int time_hour=0;
int time_minute=0;
int time_second=0;
uint8_t numsats=0;
