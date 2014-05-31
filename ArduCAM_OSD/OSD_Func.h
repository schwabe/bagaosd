
//------------------ Heading and Compass ----------------------------------------

static char buf_show[12];
const char buf_Rule[36] = {0x82,0x80,0x81,0x80,0x81,0x80,
                           0x84,0x80,0x81,0x80,0x81,0x80,
                           0x83,0x80,0x81,0x80,0x81,0x80,
                           0x85,0x80,0x81,0x80,0x81,0x80};
void setHeadingPatern()
{
  int start;
  start = round((osd_heading * 24)/360);
  start -= 3;
  if(start < 0) start += 24;
  for(int x=0; x <= 10; x++){
    buf_show[x] = buf_Rule[start];
    if(++start > 23) start = 0;
  }
  buf_show[7] = '\0';
}

//------------------ Battery Remaining Picture ----------------------------------

//Airmamaf Modif 1000 => 100 - Change image according charset 2.4
char setBatteryPic(uint16_t bat_level)
{
  if(bat_level <= 20){
    return 0x8D;
  }
  else if(bat_level <= 40){
    return 0x8C;
  }
  else if(bat_level <= 60){
    return 0x8B;
  }
  else if(bat_level <= 80){
    return 0x8A;
  }
  else return 0x89;
}

//------------------ Home Distance and Direction Calculation ----------------------------------

void setHomeVars(OSD &osd)
{
  float dstlon, dstlat;
  long bearing;
  
  osd_alt_to_home = (osd_alt - osd_home_alt);
      
  //Check arm/disarm switching.
  if (motor_armed ^ last_armed){
    //If motors armed, reset home in Arducopter version
    osd_got_home = !motor_armed;
  }
  last_armed = motor_armed;
  if(osd_got_home == 0 && osd_fix_type > 2){//TODO : use home_position_set to set gps home
    osd_home_lat = osd_lat;
    osd_home_lon = osd_lon;
    //osd_alt_cnt = 0;
    //osd_home_alt = osd_alt;
    osd_got_home = 1;
  }
  else if(osd_got_home == 1){
    // JRChange: osd_home_alt: check for stable osd_alt (must be stable for 25*120ms = 3s)
//    if(osd_alt_cnt < 25){
//      if(fabs(osd_alt_prev - osd_alt) > 0.5){
//        osd_alt_cnt = 0;
//        osd_alt_prev = osd_alt;
//      }
//      else
//      {
//        if(++osd_alt_cnt >= 25){
//          osd_home_alt = osd_alt;  // take this stable osd_alt as osd_home_alt
//          haltset = 1;
//        }
//      }
//    }
    // shrinking factor for longitude going to poles direction
    float rads = fabs(osd_home_lat) * 0.0174532925;
    double scaleLongDown = cos(rads);
    double scaleLongUp   = 1.0f/cos(rads);

    //DST to Home
    dstlat = fabs(osd_home_lat - osd_lat) * 111319.5;
    dstlon = fabs(osd_home_lon - osd_lon) * 111319.5 * scaleLongDown;
    osd_home_distance = sqrt(sq(dstlat) + sq(dstlon));
    
    if( osd_home_distance * converth  > 99999 ) {
        osd_home_distance = 99999 / converth;
    }

    //DIR to Home
    dstlon = (osd_home_lon - osd_lon); //OffSet_X
    dstlat = (osd_home_lat - osd_lat) * scaleLongUp; //OffSet Y
    bearing = 90 + (atan2(dstlat, -dstlon) * 57.295775); //absolut home direction
    if(bearing < 0) bearing += 360;//normalization
    bearing = bearing - 180;//absolut return direction
    if(bearing < 0) bearing += 360;//normalization
    bearing = bearing - osd_heading;//relative home direction
    if(bearing < 0) bearing += 360; //normalization
    osd_home_direction = ((int)round((float)(bearing/360.0f) * 16.0f) % 16) + 1;//array of arrows =)
    //if(osd_home_direction > 16) osd_home_direction = 1;
  }
}

void setFdataVars(){
  //Moved from panel because warnings also need this var and panClimb could be off
  vs = (osd_climb * converth * 60) * 0.1 + vs * 0.9;
  uint32_t  localtime = millis(); //Airmamaf, replace all millis() call by one call

  //Set startup GPS dependent variables
//  if (haltset == 1 && osd_throttle > 15){
//    haltset = 2;
//    tdistance = 0;
//  }

  //If no flight time (jittering), reset counter only
  if(osd_throttle <10 && flight_time < 3000 ) {
    flight_time = 0;
    takeofftime = 0;
  }
  //Copter specific "in flight" detection
  if(osd_throttle > 10 ) {
    //Flight time when copter is about de move (there's some throttle)
    flight_time += (localtime - last_time);
   
    if (takeofftime == 0){
      takeofftime = 1;
      tdistance = 0;
      start_battery_reading = osd_battery_remaining_A;
      last_battery_reading = osd_battery_remaining_A;
      FTime = (localtime/1000); //Airmamaf right place
    }
    start_Time = (localtime/1000) - FTime;
    not_moving_since = localtime;
    landed = 4294967295; //Airborn
  }
  //If it is stoped for more than 10 seconds, declare a landing
  else if(((localtime - not_moving_since) > 10000) && (landed == 4294967295) && (takeofftime == 1)){
    
    landed = localtime;
  }
  //FTime = (millis()/1000); //Airmamaf wrong place
  last_time = localtime;
  
  if (osd_groundspeed > 1.0) tdistance += (osd_groundspeed * (localtime - runt) / 1000.0);
  mah_used += (osd_curr_A * 10.0 * (localtime - runt) / 3600000.0);
  runt = localtime;

  //Set max data
  if (takeofftime == 1){
    if (osd_home_distance > max_home_distance) max_home_distance = osd_home_distance;
    if (osd_airspeed > max_osd_airspeed) max_osd_airspeed = osd_airspeed;
    if (osd_groundspeed > max_osd_groundspeed) max_osd_groundspeed = osd_groundspeed;
    if (osd_alt_to_home > max_osd_home_alt) max_osd_home_alt = osd_alt_to_home;
    if (osd_windspeed > max_osd_windspeed) max_osd_windspeed = osd_windspeed;
  }
}

void checkModellType(){
if (EEPROM.read(MODELL_TYPE_ADD) != 1) EEPROM.write(MODELL_TYPE_ADD, 1);
if (EEPROM.read(FW_VERSION1_ADDR) != 2) EEPROM.write(FW_VERSION1_ADDR, 2);
if (EEPROM.read(FW_VERSION2_ADDR) != 4) EEPROM.write(FW_VERSION2_ADDR, 4);
if (EEPROM.read(FW_VERSION3_ADDR) != 1) EEPROM.write(FW_VERSION3_ADDR, 1);
}

