float VRaw=0;				    //raw analog reading of voltage pin
float IRaw=0;

uint32_t total_timeFailSafe=0;      //Flight time in failsafe mode
uint32_t last_time=0;               //Time variable to compute time difference
uint32_t total_timeNoFailSafe=0;    //Flight time without failsafe
uint32_t oldFlightMode=1856;        //Handle correct flight mode transition (use for smoothing transition)
uint32_t pwmFlightMode=1856;        //Current PWM flight mode

int last_flightmode=5;              //Last flight mode before failsafe (if any failsafe)

#if defined(LIPO_CAPACITY_MAH_MULTI)
const int lipoCapacityMulti[] = {LIPO_CAPACITY_MAH_MULTI};
const byte lipo_capacity_number=sizeof(lipoCapacityMulti) / sizeof(int);
#endif

//Failsafe keeped if failsafe command for 7s
//If other mode than failsafe less than 2s and failsafe again, then total time for failsafe is keeped (no reset to 0)
boolean allowFailSafeDisable(int rcFlightMode) {
    if( !(total_timeFailSafe > 7000) ) { //Can exit Failsafe to other flight mode, if Failsafe time is less than 7s 
        return true;
    }
    
    if( total_timeFailSafe > 7000 &&  rcFlightMode != last_flightmode ) { //Can exit Failsafe to other flight mode, if flight mode different from the one before entering to Failsafe
        return true;
    }
    
    if( total_timeNoFailSafe != 0 ) { //Can exit Failsafe to other flight mode, if not disable by a long failsafe
        return true;
    }
    return false;
}

//Read PWM flight mode value, to remove intermediate value
void handleStableFlightMode() {
    uint32_t pwmFlightModeNew = rcDataSTD[FMODE_STD]; 
    
    if( abs(oldFlightMode - pwmFlightModeNew) > 4) {
        oldFlightMode = pwmFlightModeNew;
    } else {
        pwmFlightMode = pwmFlightModeNew;
    }
}

//Read flight mode : ACRO / ATTI / GPS / FSAFE
void checkFlightMode() {
 unsigned long currtime=millis();
    //Manual
  if(pwmFlightMode < FLIGHTMODE_ACRO_PMW_HIG ) { //Real limite is 1180 - 1195
    if( allowFailSafeDisable(FLIGHTMODE_ACRO_MODE) ) {
        last_flightmode = flightmode = FLIGHTMODE_ACRO_MODE; //1=Acrobatic
        total_timeNoFailSafe += (currtime - last_time);
    }
  }

  //Attitude
  if(pwmFlightMode >= FLIGHTMODE_ATTI_PMW_LOW && pwmFlightMode <FLIGHTMODE_ATTI_PMW_HIG) { //Real limite is 1510 - 1525
    if( allowFailSafeDisable(FLIGHTMODE_ATTI_MODE) ) {
        last_flightmode = flightmode = FLIGHTMODE_ATTI_MODE; //2=Alt Hold(Attitude)
        total_timeNoFailSafe += (currtime - last_time);
    }
  } 
  
  //GPS
  if(pwmFlightMode >= FLIGHTMODE_GPS_PMW_LOW ) { //Real limite is 1850 - 1865
    if( allowFailSafeDisable(FLIGHTMODE_GPS_MODE) ) {
        last_flightmode = flightmode = FLIGHTMODE_GPS_MODE; //5=Loiter(GPS Attitude)
        total_timeNoFailSafe += (currtime - last_time);
    }
  } 
       
  if( total_timeNoFailSafe > 2000 ) { //Reset failsafe flight time, when flight without failsafe is above 2s
    total_timeFailSafe = 0;
  }
  
  //Failsafe
  if((pwmFlightMode >= FLIGHTMODE_ACRO_PMW_HIG && pwmFlightMode <FLIGHTMODE_ATTI_PMW_LOW) || 
     (pwmFlightMode >= FLIGHTMODE_ATTI_PMW_HIG && pwmFlightMode <FLIGHTMODE_GPS_PMW_LOW) || 
     (total_timeNoFailSafe == 0) ) {
    flightmode = FLIGHTMODE_FAIL_MODE; //6=Return to launch(Failsafe)
    total_timeFailSafe += (currtime - last_time);
    total_timeNoFailSafe = 0;
  } 

  last_time = currtime;
}

//Read throttle percent
void checkThrottle() {
    float ret = throttle_pwm - THROTTLE_PWM_MIN; //Throttle between [1000;2000] => 0 - 1000
    ret = 100.0 * ret / (THROTTLE_PWM_MAX - THROTTLE_PWM_MIN);
    throttlepercent = constrain(ret, 0, 100);  
    
    //Switch battery capacity wtihin 20s after startup
    #if defined(LIPO_CAPACITY_MAH_MULTI)
      static unsigned long last_time=0;
      static int compute_time=0;
      static int lipo_capacity_position=0;
      unsigned long currtime=millis();
      
      if( (flight_time < BATTERY_DISPLAY_FTIME) && (currtime-last_time > 50) ){
        last_time=currtime;
        if( panel_pwm > 1800 ) {
          compute_time++;
                          
          if( compute_time > 10) {
            lipo_capacity_position++;
            compute_time=0;
            if( lipo_capacity_position >= lipo_capacity_number ) {
              lipo_capacity_position = 0;
            }
          }
          
          battery_capacity = lipoCapacityMulti[lipo_capacity_position];
          
          #if defined(DEBUG_SENSOR)
            Serial.print("POS;");
            Serial.print(lipo_capacity_position);
            Serial.print(";CAPA;");
            Serial.println(battery_capacity);
          #endif
        } else {
          compute_time = 0;
        }   
      }     
    #endif
}

//Read RSSI value
void checkRSSI(uint16_t rssiAnalog) {
    float ret = rssiAnalog;
    #if !defined(RSSI_RAW_VAL)
      ret = min(ret, RSSI_MAX_VAL);
      ret = max(ret, RSSI_MIN_VAL);
      ret = abs(100.0 * (ret - RSSI_VAL_LOW)/ (RSSI_MAX_VAL - RSSI_MIN_VAL));
    #endif

    receiver_rssi = ret; 
}

//Read voltage and current
void checkBattVolt(uint16_t voltAnalog){ 
  //Voltage sensor
  VRaw=voltAnalog;
  
  #if defined(INTERNAL_VOLTAGE_REF)
    float Vcorrected = (VRaw * intervalVCC) / INTERNAL_VOLTAGE_REF;
  #else
    float Vcorrected = VRaw;
  #endif
  
  #if defined(VOLTAGE_THROTTLE_COMP) 
    float pThottle = throttlepercent / 100.0;
    Vcorrected = Vcorrected * (1 + VOLTAGE_THROTTLE_COMP * pThottle * pThottle * pThottle);
  #endif
  
  VFinalUint.addValue(Vcorrected);
  VFinal = VFinalUint.getAverage() / VOLTAGE_FACTOR;
}

void checkBattCurrent(uint16_t currentAnalog){ 
  //Current sensor 
  #if !defined(ESTIMATE_BATTERY_REMAINING) || ESTIMATE_BATTERY_REMAINING != ENABLED
    IRaw=currentAnalog;
    
    #if defined(INTERNAL_VOLTAGE_REF)
      float Icorrected = (IRaw * intervalVCC) / INTERNAL_VOLTAGE_REF;
    #else
      float Icorrected = IRaw;
    #endif
    IFinalUint.addValue(Icorrected);
    IFinal = IFinalUint.getAverage() / CURRENT_FACTOR;
  #endif
}

void updateCurrent(unsigned long delta){ //Interval between 2 calls, in micros
	//packsize is capacity in maH
	//current present current in A
	//delta is ms
        //float delta_ms = delta / 1000.0;
	float ma_ms = IFinal * delta / 1000.0; //(Consumption in A for delta ms, it's like consumption in mA for delta s)
	float ma_hrs = ma_ms / 3600.0;
	mahout = mahout + ma_hrs;
	capacity = constrain(battery_capacity - mahout, 0, battery_capacity);
}

//Estimate remaining battery if not current sensor is used
int estimatepower(){
	int cellvolt=(VFinal*100)/LIPO_CELL;
	int remaining=5;

        if(cellvolt>335)remaining=10;
	if(cellvolt>343)remaining=20;
	if(cellvolt>350)remaining=30;
	if(cellvolt>357)remaining=40;
	if(cellvolt>363)remaining=50;
	if(cellvolt>370)remaining=60;
	if(cellvolt>377)remaining=70;
	if(cellvolt>383)remaining=80;
	if(cellvolt>390)remaining=90;
	if(cellvolt>405)remaining=100;
	return remaining;
}
