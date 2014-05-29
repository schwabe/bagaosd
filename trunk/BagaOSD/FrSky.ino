#if defined(FRSKY_PROTOCOL) 
/*
///////////////////////////////////////////////////////////////////////
// Copyright (c) 2013, Jani Hirvinen, jDrones & Co.
// All rights reserved.
//
// - Redistribution and use in source and binary forms, with or without 
//   modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation 
//  and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
//  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
//  POSSIBILITY OF SUCH DAMAGE.
//
/////////////////////////////////////////////////////////////////////////////

 
 Details:
 Program creates and populates FrSky HUB style protocol messages and feeds it out from SoftwareSerial pins on IOBoard.
 SoftwareSerial uses inverted signaling to output data correctly, if signal is non-inverted data will be corrupt due 
 XORing and shifthing one step to right process. 
 
 FrSky uses 3 frames on their HUB protocol
 
 Frame 1, every 200ms,  payload: accel-x, accel-y, accel-z, Altitude(Vario), Temp1, Temp2, Voltage (multiple), RPM
 Frame 2, every 1000ms, payload: course, lat, lon, speed, altitude (GPS), fuel level
 Frame 3, every 5000ms, payload: date, time
 
 */

#include "FrSky_vars.h"
////////////////////////////////////////////////////////////
// update_FrSky() constructing FrSky data packets
//


struct s_gps_pos {
      long   ent_pos;
      long   dec_pos;
  };

s_gps_pos fr_lon;
s_gps_pos fr_lat;

#if defined(DEBUG_FRSKY)
void test_values() {
    isArmed = 1;
  lon = 2.242697; //Ok frsky
  lat = 48.870494; //Ok frsky
  gpsFix=3; //T2 : OK (<255)
  numsats=14; //T2 : OK (<255)
  heading_d=200; //Cap : OK
  eph_cm = 154; //T1 : ok
  battery_remaining_A = 27; //Carb : ok
  ampbatt_A = 0.6; //Cour : ok
  VFinal=15.8; //Ok frsky
  
  alt_Home_m = 60;
  alt_MSL_m=100; //Alt(ok)  Dist(ko), relative Alt (AltG)
  home_set=1;
  ground_speed_ms=15; //100:338 - 184 
}
#endif

void update_FrSky() {
  f_curMillis = millis();
  if(f_curMillis - f_preMillis > f_delMillis) {
    #if defined(DEBUG_FRSKY)  
    test_values();
    #endif
    
      #if defined(FRSKY_FLD02)
        fr_speed = (ground_speed_ms * 3.6 / 1.85);
      #else
        fr_speed = ground_speed_ms;
        if( ground_speed_ms > 0) fr_speed++;
      #endif
      
      fr_volt = VFinal * 0.5238f + 0.05f; //Rounded val one digit
      fr_amp = ampbatt_A * 10.0f;
      
      fr_EW = (lon < 0 ) ? 'W' : 'E';
      fr_NS = (lat < 0) ? 'S' : 'N';
      
      fr_lon = gpsFormatConvert(lon);
      fr_lat = gpsFormatConvert(lat);

      fr_alt_msl = alt_MSL_m + 0.2f;
      
      fr_alt = (fr_alt_msl - alt_Home_m);
      
      if( VFinal > 0.1 ) {
        Batt_Cell_Detect = (byte)((VFinal-0.1)/4.2+1.0);
      }
      
    // save the last time you sent the messaga 
    f_preMillis = f_curMillis;   

    // 200ms payload, construct Frame 1 on every loop
    // Three-axis Acceleration Values, Altitude (variometer-0.01m), Tempature1, Temprature2, Voltage ,Current & Voltage (Ampere Sensor) , RPM
    packetOpen = TRUE;

    //Altitude above sea : Alt
    payloadLen += addPayload(0x10); // alt(vario). before "."
    payloadLen += addPayload(0x21); // alt, after "."
    
    //T1 : GPS eph
    payloadLen += addPayload(0x02); // Temperature 1 => eph GPS HDOP horizontal dilution of position in cm
    
    //T2 : gpsFix * 100 + number of sat.
    payloadLen += addPayload(0x05); // Temperature 2 => GPS fix + number of sat
    
    payloadLen += addPayload(0x06); // Battery data, 
    payloadLen += addPayload(0x28); // Cour : Ampere
    
    payloadLen += addPayload(0x3A); // Voltage , before "."
    payloadLen += addPayload(0x3B); // Voltage , after "."

    payloadLen += addPayload(0x03); // rpm
     
    packetOpen = FALSE;
    payloadLen = sendPayload(payloadLen);


    // 1000ms (1s) payload, construct Frame 2 on every 5th loop
    if((msCounter % 5) == 0) {
      second++;
      updateTime();
      //Course, Latitude, Longitude, Speed, Altitude (GPS), Fuel Level
      packetOpen = TRUE;
      
      //Cap
      payloadLen += addPayload(0x14);   // Course : degree
      payloadLen += addPayload(0x1c);   // Course, after "."
    
      payloadLen += addPayload(0x13);   // Longitude dddmmm 
      payloadLen += addPayload(0x1b);   // Longitude .mmmm (after ".")
      payloadLen += addPayload(0x23);   // E/W

      payloadLen += addPayload(0x12);   // Latitude dddmmm
      payloadLen += addPayload(0x1a);   // Latitude .mmmm (after ".")
      payloadLen += addPayload(0x22);   // N/S
    
      //Vit in m/s displayed in km/h or in Knots
      payloadLen += addPayload(0x11);   // GPS Speed 
      payloadLen += addPayload(0x19);   // GPS Speed after "."

      //Altitude above ground
      if( home_set == 1) {//first value sent is home_altitude, stored in receiver memory
        payloadLen += addPayload(0x01);   // GPS Altitude 
        payloadLen += addPayload(0x09);   // GPS Altitude "."
      }
      
      payloadLen += addPayload(0x04);   // Battery percent from  0 to 100
      payloadLen += addPayload(0x18);   // secs
       

      packetOpen = FALSE;
      payloadLen = sendPayload(payloadLen);
    }  


    // 5000ms (5s) payload, contruct Frame 3 on every 25th loop and reset counters
    if(msCounter >= 25) { 
      //Date, Time
      packetOpen = TRUE;
      payloadLen += addPayload(0x15); // date/month      
      payloadLen += addPayload(0x16); // year      
      payloadLen += addPayload(0x17); // hour/min      
      payloadLen += addPayload(0x18); // secs     
      packetOpen = FALSE;
      payloadLen = sendPayload(payloadLen);
      msCounter = 0;
    }
    // Update loop counter
    msCounter ++;
  }
}

////////////////////////////////////////////////////////////
// addPayload() FrSky datapacket payloads
//
byte addPayload(byte DataID) {
  
//  int test = 0;
  
  byte addedLen;
  switch(DataID) {
    case 0x01:  // GPS Altitude
      outBuff[payloadLen + 0] = 0x01;  //Signed 16 bit data
      outBuff[payloadLen + 1] = lowByte(int(fr_alt));
      outBuff[payloadLen + 2] = highByte(int(fr_alt));
      addedLen = 3;      
      break;
    case 0x01+8:  // GPS Altitude
      {
      int tmp = abs((fr_alt - int(fr_alt)) * 100.0f); //Less than 32767
      outBuff[payloadLen + 0] = 0x01+8; //Unsigned 16 bit data
      outBuff[payloadLen + 1] = lowByte(tmp);
      outBuff[payloadLen + 2] = highByte(tmp);
      addedLen = 3;      
      }
      break;

    case 0x02:  // Temperature 1 => GPS eph instead
      {
        unsigned int fr_eph = eph_cm;
        if( abs(fr_eph) > 9999) fr_eph = 9999;
        unsigned long currtime=millis();
        //Display battery size before take off
        //Otherwise display battery size in alternance with eph (every 2s)
        if( (flight_time < BATTERY_DISPLAY_FTIME) || (((currtime/1000)%4)<2) ) {
          fr_eph = battery_capacity;
        }        
        
        fr_eph = convertTemperature(fr_eph);
        outBuff[payloadLen + 0] = 0x02; //Signed 16 bit data
        outBuff[payloadLen + 1] = lowByte(fr_eph);
        outBuff[payloadLen + 2] = highByte(fr_eph);
        addedLen = 3;  
      }    
      break;

    // RPM. Works ok 24.07.13 jp. We are showing throttle value in RPM field from 1000 to 2000, same as pulse width
    // Output is scaled to FrSky displayed output which is (x * 30)  Example: 34 * 30 = 1020
    // Works as ARMED/DISARMED indicator as if DISARMED RPM value is 0
    case 0x03:  
      outBuff[payloadLen + 0] = 0x03; //Unsigned 16 bit data

      if(isArmed) {
        int fr_rpm = throttle_pwm / 30;
        outBuff[payloadLen + 1] = lowByte(fr_rpm);
        outBuff[payloadLen + 2] = highByte(fr_rpm);
      } else {
        outBuff[payloadLen + 1] = lowByte(0x00);
        outBuff[payloadLen + 2] = highByte(0x00);
      }
      addedLen = 3;      
      break;

    case 0x04:  // Fuel Level : 0, 25, 50, 75, 100
      outBuff[payloadLen + 0] = 0x04; //Unsigned 16 bit data
      outBuff[payloadLen + 1] = lowByte(battery_remaining_A);
      outBuff[payloadLen + 2] = highByte(battery_remaining_A);
      addedLen = 3;      
      break;

    // Temperature 2
    // We are using Temperature 2 to show Visible GPS satellites and also FIX type
    // Fix type is multiplied with 100 and visible satellites  is added on final number
    // For example if we have 7 satellites and we have solid 3D fix outcome will be
    // (3 * 100) + 7 = 307   (7 satellites, 3 = 3D Fix)
    case 0x05: 
      { 
        int fr_gps = 100 * gpsFix + numsats;
        unsigned long currtime=millis();
        //Alternative display if home is not set
        //Otherwise no alternate display for GPS fix and num sats 
        if( !home_set && (((currtime/1000)%4)<2) ) {
          fr_gps = 999;
        }       
        
        fr_gps = convertTemperature(fr_gps);
        outBuff[payloadLen + 0] = 0x05; //Signed 16 bit data
        outBuff[payloadLen + 1] = lowByte(fr_gps);
        outBuff[payloadLen + 2] = highByte(fr_gps);
        addedLen = 3;  
      }    
      break;

    //Little Endian exception
    case 0x06:  // Voltage, first 4 bits are cell number, rest 12 are voltage in 1/500v steps, scale 0-4.2v
      //==================Change Data Batt volt for Sending===========//
       if(Frsky_Count_Order_Batt < Batt_Cell_Detect) {
          Frsky_Batt_Volt_A=((((VFinal/Batt_Cell_Detect)*2100)/4.2));
          
         // the first 4 bit of the voltage data refers to battery cell number, 
         // while the last 12 bit refers to the
         // voltage value. 0-2100 corresponding to 0-4.2V.
         outBuff[payloadLen + 0] = 0x06;
         outBuff[payloadLen + 1] = (Frsky_Count_Order_Batt<<4)&0xF0 | ((Frsky_Batt_Volt_A>>8)&0x0F);  
         outBuff[payloadLen + 2] = (Frsky_Batt_Volt_A)&0xFF;
         
         Frsky_Count_Order_Batt++;
         addedLen = 3;
       } else {
         Frsky_Count_Order_Batt=0;
         addedLen=0;
       }
      break;
      
    case 0x10:  // Altitude, before "." works on FLD-02, Taranis no (Always >= 0)
      outBuff[payloadLen + 0] = 0x10; //Unsigned 16 bit data
      outBuff[payloadLen + 1] = lowByte(long(fr_alt_msl)); 
      outBuff[payloadLen + 2] = highByte(long(fr_alt_msl));
      addedLen = 3;      
      break;
    case 0x21:  // Altitude, after "."
      {
        int tmp = (fr_alt_msl - int(fr_alt_msl)) * 100.0f; //Less than 32767
        outBuff[payloadLen + 0] = 0x21;
        outBuff[payloadLen + 1] = lowByte(tmp);
        outBuff[payloadLen + 2] = highByte(tmp);
        addedLen = 3;      
      }
      break;

      
    case 0x11:  // GPS Speed, before "." Always >= 0
      outBuff[payloadLen + 0] = 0x11; //Unsigned 16 bit data
      outBuff[payloadLen + 1] = lowByte(long(fr_speed));
      outBuff[payloadLen + 2] = highByte(long(fr_speed));
      addedLen = 3;      
      break;
    case 0x11+8:  // GPS Speed, after "." => Not displayed 
      {
        outBuff[payloadLen + 0] = 0x11+8; //Unsigned 16 bit data
        outBuff[payloadLen + 1] = 0x00;//FixInt(tmp, 1); //0x00;
        outBuff[payloadLen + 2] = 0x00;//FixInt(tmp, 2); //0x00;
        addedLen = 3;      
      }
      break;

    //Little Endian exception
    case 0x12:  // Longitude, before "." //dddmm.mmmm
      {
        outBuff[payloadLen + 0] = 0x12;
        outBuff[payloadLen + 1] = lowByte(fr_lon.ent_pos);
        outBuff[payloadLen + 2] = highByte(fr_lon.ent_pos);
        addedLen = 3;    
      }  
      break;
    case 0x12+8:  // Longitude, after "."
      {
        outBuff[payloadLen + 0] = 0x12+8;
        outBuff[payloadLen + 1] = lowByte(fr_lon.dec_pos);  // Only allow .0000 4 digits
        outBuff[payloadLen + 2] = highByte(fr_lon.dec_pos);  // Only allow .0000 4 digits after .
        addedLen = 3;      
      }
      break;

    case 0x1A+8:  // E/W
      outBuff[payloadLen + 0] = 0x1A+8;
      outBuff[payloadLen + 1] = fr_EW;
      outBuff[payloadLen + 2] = 0;
      addedLen = 3;      
      break;

      
    //Little Endian exception
    case 0x13:  // Latitude, before "." //ddmm.mmmm
      {
        outBuff[payloadLen + 0] = 0x13;
        outBuff[payloadLen + 1] = lowByte(fr_lat.ent_pos);
        outBuff[payloadLen + 2] = highByte(fr_lat.ent_pos);
        addedLen = 3;      
      }
      break;
    case 0x13+8:  // Latitude, after "."
      {
        outBuff[payloadLen + 0] = 0x13+8;
        outBuff[payloadLen + 1] = lowByte(fr_lat.dec_pos);  // Only allow .0000 4 digits
        outBuff[payloadLen + 2] = highByte(fr_lat.dec_pos);  // Only allow .0000 4 digits after .   
        addedLen = 3;      
      }
      break;  

    case 0x1B+8:  // N/S
      outBuff[payloadLen + 0] = 0x1B+8;
      outBuff[payloadLen + 1] = fr_NS;
      outBuff[payloadLen + 2] = 0;      
      addedLen = 3;      
      break;

    case 0x14:  // course, before ".". OK
      outBuff[payloadLen + 0] = 0x14;
      outBuff[payloadLen + 1] = lowByte(int(heading_d));
      outBuff[payloadLen + 2] = highByte(int(heading_d));
      addedLen = 3;      
      break;
    case 0x14+8:  // course, after "."  .. check calculation
      {
        int tmp = abs(heading_d - int(heading_d)) * 1000.0f; //Less than 32767
        outBuff[payloadLen + 0] = 0x14+8;
        outBuff[payloadLen + 1] = lowByte(tmp);
        outBuff[payloadLen + 2] = highByte(tmp);
        addedLen = 3;      
      }
      break;
      
    case 0x15: // date/month
      outBuff[payloadLen + 0] = 0x15;
      outBuff[payloadLen + 1] = 0x00;
      outBuff[payloadLen + 2] = 0x00;
      addedLen = 3;      
      break;
    case 0x16: // year
      outBuff[payloadLen + 0] = 0x16;
      outBuff[payloadLen + 1] = 0x00;
      outBuff[payloadLen + 2] = 0x00;
      addedLen = 3;      
      break;
    case 0x17: // hour/minute
      outBuff[payloadLen + 0] = 0x17;
      outBuff[payloadLen + 1] = hour, DEC;
      outBuff[payloadLen + 2] = minutes, DEC;
      addedLen = 3;      
      break;
    case 0x18: // second
      outBuff[payloadLen + 0] = 0x18;
      outBuff[payloadLen + 1] = second, DEC;
      outBuff[payloadLen + 2] = 0x00;
      addedLen = 3;      
      break;

    case 0x3A:  // Volt 
      //iob_vbat_A o boardVoltage
      outBuff[payloadLen + 0] = 0x3A; //Always >= 0
      outBuff[payloadLen + 1] = lowByte(int(fr_volt));
      outBuff[payloadLen + 2] = highByte(int(fr_volt));
      addedLen = 3;      
      break;
    case 0x3B:
      {
        int tmp = (fr_volt - int(fr_volt) + 0.05f) * 10.0f;
        outBuff[payloadLen + 0] = 0x3B;
        outBuff[payloadLen + 1] = lowByte(tmp);
        outBuff[payloadLen + 2] = highByte(tmp);
      
        addedLen = 3;      
      }
      break;

    case 0x28:
      outBuff[payloadLen + 0] = 0x28;
      outBuff[payloadLen + 1] = lowByte(int(fr_amp));
      outBuff[payloadLen + 2] = highByte(int(fr_amp));
      addedLen = 3;      
      break;
      
    default:
      addedLen = 0;
  }
  return addedLen; 

}

byte addEnd() {
 return 1; 
}

// Sending packets. Create frame with correct data
// Frame format:
//

byte sendPayload(byte len) {
  
  sendTelemetry.write(0x5E);
  for(byte pos = 0; pos <= len-1 ; pos = pos + 3) {
    sendTelemetry.write(byte(outBuff[pos + 0]));

    switch  (outBuff[pos + 1]) {
      case 0x5E:
        sendTelemetry.write(byte(0x5D));
        sendTelemetry.write(byte(0x3E));
        break;
      case 0x5D:
        sendTelemetry.write(byte(0x5D));
        sendTelemetry.write(byte(0x3D));
        break;
        
      default:
        sendTelemetry.write(byte(outBuff[pos + 1]));
    }

    switch  (outBuff[pos + 2]) {
      case 0x5E:
        sendTelemetry.write(byte(0x5D));
        sendTelemetry.write(byte(0x3E));
        break;
      case 0x5D:
        sendTelemetry.write(byte(0x5D));
        sendTelemetry.write(byte(0x3D));
        break;        
      default:
        sendTelemetry.write(byte(outBuff[pos + 2]));
    }
    sendTelemetry.write(0x5E);
  }
  return 0;
}


////////////////////////////////////////////////////////////
// updateTime() Time counters for FrSky telemetry
//
void updateTime() {
  if(second >= 60) {
    second = 0;
    minutes++;
   }
   if(minutes >= 60) {
    second = 0;
    minutes = 0;
    hour++;
   } 
   if(hour >= 24) {
     second = 0;
     minutes = 0;
     hour = 0;
   }
}

////////////////////////////////////////////////////////////
// Convert GPS format from Google Format to HMS (100*H + M + S/100)

#if defined(FRSKY_DMS) && !defined(FRSKY_FLD02)
const float secgps_fac = 100.0;
#else
const float secgps_fac = 60.0;
#endif

struct s_gps_pos gpsFormatConvert(float gpsPosIn) {
  s_gps_pos gps_conv;
  float gpsPos = abs(gpsPosIn);
  
  long entGpsPos = long(gpsPos);
  float minGpsPos = (gpsPos - entGpsPos) * 60.0;
  gps_conv.ent_pos = entGpsPos * 100 + long(minGpsPos);
  gps_conv.dec_pos = ((minGpsPos - long(minGpsPos)) * secgps_fac + 0.005)*100; //Round 

  return gps_conv;
}


long convertTemperature(long temp) {
#if defined(FRSKY_IMPERIALS) 
return (temp - 32.0)/1.8+0.5;
#else
return temp;
#endif
}

#endif
