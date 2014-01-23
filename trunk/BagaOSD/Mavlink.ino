/**
MinimOSD standard configuration
https://code.google.com/p/minimosd-extra/wiki/APM

- 2hz for waypoints, GPS raw, fence data, current waypoint, etc
- 5hz for attitude and simulation state
- 2hz for VFR_Hud data 
- 3hz for AHRS, Hardware Status, Wind 
- 2hz for location data 
- 2hz for raw imu sensor data 
- 5hz for radio input or radio output data 

**/


void sendMavlinkMessages() {
  unsigned long currtime=millis();
  
  static unsigned long heartbeat_1hz=0;
  static unsigned long heartbeat_2hz=0;
  static unsigned long heartbeat_3hz=0;
  static unsigned long heartbeat_5hz=0;
  static unsigned long dimming = 0;

  mavlink_system.sysid = 100; // System ID, 1-255
  mavlink_system.compid = 50; // Component/Subsystem ID, 1-255
  
  //1hz for mavlink heart beat
  if(currtime - heartbeat_1hz >= 1000){
    sendHeartBeat();
    heartbeat_1hz = currtime;
    dimming = currtime;
  } 
  
  //2hz for waypoints, GPS raw, fence data, current waypoint, etc
  //2hz for VFR_Hud data 
  if(currtime - heartbeat_2hz >= 500){
    sendGpsData();
    sendVfrHud();
    heartbeat_2hz = currtime;
    dimming = currtime;
  } 
  
  //3hz for AHRS, Hardware Status, Wind 
  if(currtime - heartbeat_3hz >= 333){
    sendSystemStatus();
    heartbeat_3hz = currtime;
    dimming = currtime;
  } 
  
  //5hz for attitude and simulation state
  //5hz for radio input or radio output data 
  if(currtime - heartbeat_5hz >= 200){
    sendAttitude();
    sendRawChannels();
    heartbeat_5hz = currtime;
    dimming = currtime;
  } 
  
  if( currtime - dimming < 15 ) { //LED is on for 30ms when a message is sent
    LEDPIN_ON
  } else {
    LEDPIN_OFF
  }
}

//Send MAVLink Heartbeat, 1hz 
void sendHeartBeat() {
  //static inline void mavlink_msg_heartbeat_send(mavlink_channel_t chan, uint8_t type, uint8_t autopilot, uint8_t base_mode, uint32_t custom_mode, uint8_t system_status)
  //flightmode 0 : Stabilize
  //flightmode 1 : Acrobatic
  //flightmode 2 : Alt Hold
  //flightmode 3 : Auto
  //flightmode 4 : Guided
  //flightmode 5 : Loiter
  //flightmode 6 : Return to Launch
  //flightmode 7 : Circle
  //flightmode 8 : Position
  //flightmode 9 : Land
  //flightmode 10 : OF_Loiter
  mavlink_msg_heartbeat_send(
    MAVLINK_COMM_0, 
    MAV_TYPE_QUADROTOR, 
    MAV_AUTOPILOT_GENERIC, 
    MAV_MODE_FLAG_STABILIZE_ENABLED | MAV_MODE_FLAG_GUIDED_ENABLED | MAV_MODE_FLAG_SAFETY_ARMED | MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | MAV_MODE_FLAG_MANUAL_INPUT_ENABLED, 
    flightmode, 
    MAV_STATE_STANDBY);
}



//Send GPS data
void sendGpsData() {
  //mavlink_msg_gps_raw_int_send(MAVLINK_COMM_0,gps_utc_time_second, gpsFix, (long)(lat*10000000), (long)(lon*10000000), alt_MSL, vdop,hdop, ground_speed, heading, numsats);
  //MAVLINK_MSG_ID_GPS_RAW_INT
  mavlink_msg_gps_raw_int_send(
    MAVLINK_COMM_0,
    millis(), 
    gpsFix, 
    (long)(lat*10000000), 
    (long)(lon*10000000), 
    alt_MSL, 
    vdop,
    hdop, 
    ground_speed, 
    heading, 
    numsats);
}

//Send horizon position
void sendAttitude() {
	pitch_rad=(levelpitch-pitch)*3.1415/500.0 * pitchgain/12.00;      //500 is the difference between vertical and level
	if(invertpitch) pitch_rad=pitch_rad*-1;
	//need to scale up or down the pitch and roll - a delta of 500 is correct if pitch = 12 and roll is 7.6.  So scale the inputted values by that?
	//Pitch correction factor = configured pitch gain /12.00
	//Roll correction factor = configured roll gain / 7.60
	roll_rad=(levelroll-roll)*-3.1415/500.0 * rollgain/7.60;
	if(invertroll) roll_rad=roll_rad*-1;
	if(pitch_rad<.05 && pitch_rad>-.05) pitch_rad=0;
	if(roll_rad<.05 && roll_rad > -.05) roll_rad=0;
	if(pitch_rad>5) pitch_rad=0;
	if(roll_rad>5) roll_rad=0;
	if(pitch_rad<-5) pitch_rad=0;
	if(roll_rad<-5) roll_rad=0;
	mavlink_msg_attitude_send(
          MAVLINK_COMM_0, 
          millis(), 
          roll_rad, 
          pitch_rad,
          0.0, 
          0, 
          0, 
          0);
}

//Send speed, orientation, altitude and throttle percent
void sendVfrHud() {
	//mavlink_msg_vfr_hud_send(mavlink_channel_t chan, float airspeed, float groundspeed, int16_t heading, uint16_t throttle, float alt, float climb)
	//mavlink_msg_vfr_hud_send(MAVLINK_COMM_0,ground_speed,ground_speed, ground_course, 0, alt_MSL, 0);

        //MAVLINK_MSG_ID_VFR_HUD
	mavlink_msg_vfr_hud_send(
          MAVLINK_COMM_0,
          ground_speed,
          ground_speed, 
          heading, 
          throttlepercent, 
          alt_MSL, 
          -climb); 
}

//Sensor information : current, voltage
void sendSystemStatus() {
	//current is in ma.  Function needs to send in ma/10.
	#if ESTIMATE_BATTERY_REMAINING == ENABLED
	    mavlink_msg_sys_status_send(MAVLINK_COMM_0,0,0,0,0,long(VFinal*1000.0),0 ,estimatepower(),0,0,0,0,0,0); 
	#else
	    mavlink_msg_sys_status_send(MAVLINK_COMM_0,0,0,0,0,long(VFinal*1000.0),IFinal*100.0,capacity/LIPO_CAPACITY_MAH*100.0,0,0,0,0,0,0);
    #endif
}

//Send RAW channels data and RSSI
void sendRawChannels() {
	mavlink_msg_rc_channels_raw_send(
        MAVLINK_COMM_0,
        millis(),
        0,        // port 0
        rcDataSTD[GIMBALROLL_STD],
        rcDataSTD[GIMBALPITCH_STD],
        rcDataSTD[YAW_STD],
        rcDataSTD[THROTTLE_STD],
        rcDataSTD[FMODE_STD],
        0,
        0,
        0,
        receiver_rssi);
}

