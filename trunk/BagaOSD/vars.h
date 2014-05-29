//Flight mode
int flightmode=5;               //2=Alt Hold(Attitude), 1=Acrobatic, 6=Return to launch(Failsafe), 5=Loiter(GPS Attitude);

//Gimbal position 
float pitch_rad=0;
float roll_rad=0;

float capacity=0;
float mahout=0;

//Other sensors
int receiver_rssi=0;
int throttlepercent=0;


/*GPS variables*/
int gpsFix=1;
float lat=0;
float lon=0;
float alt_MSL_m=0;
long iTOW=0;
float alt_m=0;
short eph_cm=65535;
short epv_cm=65535;
float speed_3d=0;
float ground_speed_ms=0;
float heading_d=90.0;
float cog_cd=heading_d * 100.0;
float climb=0;
char data_update_event=0;
short time_year=0;
int time_month=0;
int time_day=0;
int time_hour=0;
int time_minute=0;
int time_second=0;
uint8_t numsats=0;
unsigned long fix_time=0;
float alt_Home_m=0;
int home_set=0;

//For telemetry
int isArmed = 0;     // Is motors armed flag
int throttle_pwm=0; 
int roll_pwm=0;
int pitch_pwm=0;
int panel_pwm=0;
unsigned long flight_time=0;

uint16_t battery_remaining_A = 0;
float ampbatt_A = 0; //Conso courante

long intervalVCC=1;

int battery_capacity = LIPO_CAPACITY_MAH;

