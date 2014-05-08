float VRaw=0;				    //raw analog reading of voltage pin
float IRaw=0;

uint32_t total_timeFailSafe=0;      //Flight time in failsafe mode
uint32_t last_time=0;               //Time variable to compute time difference
uint32_t total_timeNoFailSafe=0;    //Flight time without failsafe
uint32_t oldFlightMode=1856;        //Handle correct flight mode transition (use for smoothing transition)
uint32_t pwmFlightMode=1856;        //Current PWM flight mode

int last_flightmode=5;              //Last flight mode before failsafe (if any failsafe)

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
  uint32_t localtime = millis(); 
  
    //Manual
  if(pwmFlightMode < FLIGHTMODE_ACRO_PMW_HIG ) { //Real limite is 1180 - 1195
    if( allowFailSafeDisable(FLIGHTMODE_ACRO_MODE) ) {
        last_flightmode = flightmode = FLIGHTMODE_ACRO_MODE; //1=Acrobatic
        total_timeNoFailSafe += (localtime - last_time);
    }
  }

  //Attitude
  if(pwmFlightMode >= FLIGHTMODE_ATTI_PMW_LOW && pwmFlightMode <FLIGHTMODE_ATTI_PMW_HIG) { //Real limite is 1510 - 1525
    if( allowFailSafeDisable(FLIGHTMODE_ATTI_MODE) ) {
        last_flightmode = flightmode = FLIGHTMODE_ATTI_MODE; //2=Alt Hold(Attitude)
        total_timeNoFailSafe += (localtime - last_time);
    }
  } 
  
  //GPS
  if(pwmFlightMode >= FLIGHTMODE_GPS_PMW_LOW ) { //Real limite is 1850 - 1865
    if( allowFailSafeDisable(FLIGHTMODE_GPS_MODE) ) {
        last_flightmode = flightmode = FLIGHTMODE_GPS_MODE; //5=Loiter(GPS Attitude)
        total_timeNoFailSafe += (localtime - last_time);
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
    total_timeFailSafe += (localtime - last_time);
    total_timeNoFailSafe = 0;
  } 

  last_time = localtime;
}

//Read throttle percent
void checkThrottle() {
    float ret = rcDataSTD[THROTTLE_STD] - THROTTLE_PWM_MIN; //Throttle between [1000;2000] => 0 - 1000
    ret = 100.0 * ret / (THROTTLE_PWM_MAX - THROTTLE_PWM_MIN);
    throttlepercent = constrain(ret, 0, 100);
}

//Read RSSI value
void checkRSSI() {
    float ret = analogValues[RSSI_PIN_ANALOG_POS];
    #if !defined(RSSI_RAW_VAL) || RSSI_RAW_VAL != TRU
      ret = min(ret, RSSI_MAX_VAL);
      ret = max(ret, RSSI_MIN_VAL);
      ret = abs(100.0 * (ret - RSSI_VAL_LOW)/ (RSSI_MAX_VAL - RSSI_MIN_VAL));
    #endif

    RFinalUint.addValue(ret);
    receiver_rssi = RFinalUint.getAverage(); 
}

//Read voltage and current
void checkBattVolt(){ 
  //Voltage sensor
  VRaw=analogValues[VOLT_PIN_ANALOG_POS];
  
  #if defined(INTERNAL_VOLTAGE_REF)
    float Vcorrected = (VRaw * intervalVCC) / INTERNAL_VOLTAGE_REF;
  #else
    float Vcorrected = VRaw;
  #endif
  
  #if defined(VOLTAGE_THROTTLE_COMP) 
    float pThottle = throttlepercent / 100;
    Vcorrected = Vcorrected * (1 + VOLTAGE_THROTTLE_COMP * pThottle * pThottle * pThottle);
  #endif
  
  VFinalUint.addValue(Vcorrected);
  VFinal = VFinalUint.getAverage() / VOLTAGE_FACTOR;

  //Current sensor 
  #if !defined(ESTIMATE_BATTERY_REMAINING) || ESTIMATE_BATTERY_REMAINING != ENABLED
    IRaw=analogValues[CURR_PIN_ANALOG_POS];
    
    #if defined(INTERNAL_VOLTAGE_REF)
      float Icorrected = (IRaw * intervalVCC) / INTERNAL_VOLTAGE_REF;
    #else
      float Icorrected = IRaw;
    #endif
    IFinalUint.addValue(Icorrected);
    IFinal = IFinalUint.getAverage() / CURRENT_FACTOR;
  #endif
  
  #if defined(DEBUG_SENSOR)
	static unsigned long lastchecked=0;
	if(millis()-lastchecked>1000){
		Serial.print("Current voltage: ");
		Serial.print(VFinal);
                Serial.print("  <>  ");
                Serial.print(VFinalUint.getAverage());
                Serial.print("  <>  ");
                Serial.println(intervalVCC);
		
                #if ESTIMATE_BATTERY_REMAINING == ENABLED
		  Serial.print("Estimated pack remaining (%): ");
		  Serial.println(estimatepower());
                #else
                  Serial.print("Current current: ");
		  Serial.print(IFinal);
                  Serial.print("  <>  ");
                  Serial.print(IFinalUint.getAverage());
                  Serial.print("  <>  ");
                  Serial.println(intervalVCC);
                #endif
                Serial.println("----------------");
		lastchecked=millis();
	}
  #endif
}

void updateCurrent(int delta){ //Interval between 2 calls, in ms
	//packsize is capacity in maH
	//current present current in A
	//delta is ms
	float ma_ms = IFinal * delta; //(Consumption in A for delta ms, it's like consumption in mA for delta/1000 s)
	float ma_hrs = ma_ms / 3600;
	mahout = mahout + ma_hrs;
	capacity = constrain(LIPO_CAPACITY_MAH - mahout, 0, LIPO_CAPACITY_MAH);

        #if defined(DEBUG_SENSOR)
	static unsigned long lastchecked=0;
	if(millis()-lastchecked>1000){
		Serial.print("Consumption: ");
		Serial.print(mahout);
                Serial.print("  <> remaining: ");
                Serial.println(capacity);
		lastchecked=millis();
	}
        #endif
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
