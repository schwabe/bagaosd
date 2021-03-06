/*
  DJI Naza (v1, v1 Lite, V2) data decoder library
  (c) Pawelsky 20140123
  Not for commercial use

  Refer to naza_decoder_wiring.jpg diagram for proper connection

  The RC PWM input code taken from https://www.instructables.com/id/RC-Quadrotor-Helicopter/step12/Arduino-Demo-PWM-Input/
  
  
  Airmamaf : This version is a modified version to meet BagaOSD needs
  Original version can be found here http://www.rcgroups.com/forums/showthread.php?t=1995704
*/

#include "Arduino.h"
#include "NazaDecoderLib.h"

NazaDecoderLib NazaDecoder;

NazaDecoderLib::NazaDecoderLib()
{
  seq = 0;
  cnt = 0;
  msgId = 0;
  msgLen = 0;
  cs1 = 0;
  cs2 = 0;
  //startPwmReader(); //Airmamaf : disabled, not need BagaOSD
}

int32_t NazaDecoderLib::decodeLong(uint8_t idx, uint8_t mask)
{
  union { uint32_t l; uint8_t b[4]; } val;
  for(int i = 0; i < 4; i++) val.b[i] = gpsPayload[idx + i] ^ mask;
  return val.l;
}

int16_t NazaDecoderLib::decodeShort(uint8_t idx, uint8_t mask)
{
  union { uint16_t s; uint8_t b[2]; } val;
  for(int i = 0; i < 2; i++) val.b[i] = gpsPayload[idx + i] ^ mask;
  return val.s;
}

void NazaDecoderLib::updateCS(int input)
{
  cs1 += input;
  cs2 += cs1;
}

double NazaDecoderLib::getLat() { return gpsData.lat; }
double NazaDecoderLib::getLon() { return gpsData.lon; }
double NazaDecoderLib::getAlt() { return gpsData.alt; }
double NazaDecoderLib::getClimbSpeed() { return gpsData.clb; } //Airmamaf add climb speed in m/s
double NazaDecoderLib::getSpeed() { return gpsData.spd; }
NazaDecoderLib::fixType_t  NazaDecoderLib::getFixType() { return gpsData.fix; }
uint8_t NazaDecoderLib::getNumSat() { return gpsData.sat; }
double NazaDecoderLib::getHeading() { return gpsData.heading; }
double NazaDecoderLib::getCog() { return gpsData.cog; }
double NazaDecoderLib::getHdop() { return gpsData.hdop; }
double NazaDecoderLib::getVdop() { return gpsData.vdop; } //Airmamaf add vdop information
uint8_t NazaDecoderLib::getYear() { return gpsData.year; }
uint8_t NazaDecoderLib::getMonth() { return gpsData.month; }
uint8_t NazaDecoderLib::getDay() { return gpsData.day; }
uint8_t NazaDecoderLib::getHour() { return gpsData.hour; }
uint8_t NazaDecoderLib::getMinute() { return gpsData.minute; }
uint8_t NazaDecoderLib::getSecond() { return gpsData.second; }

void NazaDecoderLib::setCompensation(double pitch, double roll) { //Airmamaf tilt compensation
    rad_pitch_cmp = pitch;
    rad_roll_cmp = roll; 
}

uint8_t NazaDecoderLib::decode(int input)
{ 
  if((seq == 0) && (input == 0x55)) { seq++; }                                                             // header (part 1 - 0x55)
  else if((seq == 1) && (input == 0xAA)) { cs1 = 0; cs2 = 0; seq++; }                                     // header (part 2 - 0xAA) 
  else if((seq == 2) && ((input == 0x10) || (input == 0x20))) { msgId = input; updateCS(input); seq++; }  // message id
  else if(seq == 3) { msgLen = input; cnt = 0; updateCS(input); seq++; }                                  // message payload lenght
  else if(seq == 4) { gpsPayload[cnt++] = input; updateCS(input); if(cnt >= msgLen) { seq++; } }          // store payload in buffer
  else if((seq == 5) && (input == cs1)) { seq++; }                                                        // verify checksum #1
  else if((seq == 6) && (input == cs2)) { seq++; }                                                        // verify checksum #2
  else seq = 0;

  if(seq == 7) // all data in buffer
  {
    seq = 0;
    // Decode GPS data
    if(msgId == NAZA_MESSAGE_GPS)
    {
      uint8_t mask = gpsPayload[55];
      uint32_t time = decodeLong(0, mask);
      gpsData.second = time & 0b00111111; time >>= 6;
      gpsData.minute = time & 0b00111111; time >>= 6;
      gpsData.hour = time & 0b00001111; time >>= 4;
      gpsData.day = time & 0b00011111; time >>= 5;
      gpsData.month = time & 0b00001111; time >>= 4;
      gpsData.year = time & 0b01111111;
      gpsData.lon = (double)decodeLong(4, mask) / 10000000;
      gpsData.lat = (double)decodeLong(8, mask) / 10000000;
      gpsData.alt = (double)decodeLong(12, mask) / 1000;
      double nVel = (double)decodeLong(28, mask) / 100; 
      double eVel = (double)decodeLong(32, mask) / 100;
	  gpsData.clb  = (double)decodeLong(36, mask) / 100; //Airmamaf add climb speed
      gpsData.spd  = sqrt(nVel * nVel + eVel * eVel); //Airmamaf leave information in m/s  * 1.943884; // convert from m/s to knots
      gpsData.cog = atan2(eVel, nVel) * 180.0 / M_PI;
      if(gpsData.cog < 0) gpsData.cog += 360.0;
	  gpsData.vdop = (double)decodeShort(42, mask) / 100; //Airmamaf add vdop information
      double ndop = (double)decodeShort(44, mask) / 100; 
      double edop = (double)decodeShort(46, mask) / 100;
      gpsData.hdop = sqrt(ndop * ndop + edop * edop);
      gpsData.sat  = gpsPayload[48];
      uint8_t fixType = gpsPayload[50] ^ mask;
      uint8_t fixFlags = gpsPayload[52] ^ mask;
      switch(fixType)
      {
        case 2 : gpsData.fix = FIX_2D; break;
        case 3 : gpsData.fix = FIX_3D; break;
        default: gpsData.fix = NO_FIX; break;
      }
      if((gpsData.fix != NO_FIX) && (fixFlags & 0x02)) gpsData.fix = FIX_DGPS;
    }
    // Decode compass data (not tilt compensated)
    else if (msgId == NAZA_MESSAGE_COMPASS)
    {
      uint8_t mask = gpsPayload[4];
      mask = (((mask ^ (mask >> 4)) & 0x0F) | ((mask << 3) & 0xF0)) ^ (((mask & 0x01) << 3) | ((mask & 0x01) << 7)); 
      int16_t x = decodeShort(0, mask);
      int16_t y = decodeShort(2, mask);
      int16_t z = decodeShort(4, mask); //Airmamaf for tilt compensation
      /*
      if(x > magXMax) magXMax = x;
      if(x < magXMin) magXMin = x;
      if(y > magYMax) magYMax = y;
      if(y < magYMin) magYMin = y;
      */
      
      //Airmamaf Tilt compensation
      double comp_x = x*cos(rad_pitch_cmp) + z*sin(rad_pitch_cmp);
      double comp_y = x*sin(rad_roll_cmp)*sin(rad_pitch_cmp) + y*cos(rad_roll_cmp) - z*sin(rad_roll_cmp)*cos(rad_pitch_cmp);
      /*if(comp_x > magXMax) magXMax = comp_x;
      if(comp_x < magXMin) magXMin = comp_x;
      if(comp_y > magYMax) magYMax = comp_y;
      if(comp_y < magYMin) magYMin = comp_y;*/
      
      gpsData.heading = atan2(comp_y, comp_x) * 180.0 / M_PI;
      //gpsData.heading = atan2(y - ((magYMax + magYMin) / 2), x - ((magXMax + magXMin) / 2)) * 180.0 / M_PI;

      if(gpsData.heading < 0) gpsData.heading += 360.0; 
    }
    return msgId;
  }
  else
  {
    return NAZA_MESSAGE_NONE;
  }
}

//Airmamaf : disabled, not need BagaOSD
/*
#define PIN_MASK     0b00001100

void NazaDecoderLib::startPwmReader()
{
  pinMode(2, INPUT_PULLUP); // Pitch (Arduino D2 <-> Naza F2)
  pinMode(3, INPUT_PULLUP); // Roll  (Arduino D3 <-> Naza F1)
  cli();
  PCICR = 1 << PCIE2;
  PCMSK2 = PIN_MASK;
  sei();
}

void NazaDecoderLib::pwmInterruptHandler()
{
  uint8_t bit;
  uint8_t curr;
  uint8_t mask;
  uint32_t currentTime;
  uint32_t time;

  // get the pin states for the indicated port.
  curr = PIND & PIN_MASK;
  mask = curr ^ pcIntLast;
  pcIntLast = curr;

  currentTime = micros();

  // mask is pcint pins that have changed.
  for (uint8_t i = 0; i < 2; i++) {
    bit = 0b00000100 << i;
    if (bit & mask) {
      // for each pin changed, record time of change
      if (bit & pcIntLast) {
        time = currentTime - pwmData[i].fallTime;
        pwmData[i].riseTime = currentTime;
        if ((time >= 10000) && (time <= 26000))
          pwmData[i].edge = 1;
        else
          pwmData[i].edge = 0; // invalid rising edge detected
      }
      else {
        time = currentTime - pwmData[i].riseTime;
        pwmData[i].fallTime = currentTime;
        if ((time >= 800) && (time <= 2200) && (pwmData[i].edge == 1)) {
          pwmData[i].lastGoodWidth = time;
          pwmData[i].edge = 0;
        }
      }
    }
  }
}

ISR(PCINT2_vect)
{
  NazaDecoder.pwmInterruptHandler();
}

int8_t NazaDecoderLib::pwm2Deg(uint32_t pulseWidth)
{
  return (pulseWidth == 0) ? 0 : map(constrain(pulseWidth, 1000, 2000), 1000, 2000, -90, 90);
}

int8_t  NazaDecoderLib::getPitch()
{
  return pwm2Deg(pwmData[0].lastGoodWidth);
}

int8_t  NazaDecoderLib::getRoll()
{
  return pwm2Deg(pwmData[1].lastGoodWidth);
}
*/
