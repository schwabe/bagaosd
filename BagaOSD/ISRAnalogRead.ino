#define DELAY_ANALOG  10

volatile int adcReading;
volatile boolean adcDone;
boolean adcStarted;
byte adcPin;
byte lastPin=NO_ANALOG_PIN;

long last_read;

//VCC Internal
boolean init_done = false;


//Numeric values for analog pin
uint8_t analogPin[NUMBER_ANALOG_PIN] = {ANALOG_PIN_DEF_VAL};


void init_analog() {
  #if defined(RSSI_PIN_ANALOG) 
    if( RSSI_PIN_ANALOG != -1 ) {
      analogPin[RSSI_PIN_ANALOG_POS] = digitalPinToPCMSKbit(RSSI_PIN_ANALOG);
    }
  #endif
  #if defined(VOLTAGE_PIN)
    analogPin[VOLT_PIN_ANALOG_POS] = digitalPinToPCMSKbit(VOLTAGE_PIN);
  #endif
  #if defined(CURRENT_PIN)
    analogPin[CURR_PIN_ANALOG_POS] = digitalPinToPCMSKbit(CURRENT_PIN);
  #endif
}

// ADC complete ISR
ISR (ADC_vect) {
    byte low, high;
    // we have to read ADCL first; doing so locks both ADCL
    // and ADCH until ADCH is read. reading ADCL second would
    // cause the results of each conversion to be discarded,
    // as ADCL and ADCH would be locked when it completed.
    low = ADCL;
    high = ADCH;
    adcReading = (high << 8) | low;
    adcDone = true;
} // end of ADC_vect

void read_analog(long lread) {
  // if last reading finished, process it
  if (adcDone) {
    if( !init_done) {
      // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000 or scale_constant = internal1.1Ref * 1023 * 1000
      // internal1.1Ref = 1.1 * Vcc1 (per voltmeter) / Vcc2 (per readVcc() function)
      intervalVCC = 1125300L / adcReading;  
      init_done = true;
      last_read = lread;
    } else {
      if( lastPin != NO_ANALOG_PIN ) {
        analogValues[lastPin] = adcReading;
      }
        
      if( lread - last_read > DELAY_ANALOG) {
        if( adcPin > NUMBER_ANALOG_PIN) adcPin = 0;
        if( analogPin[adcPin] != NO_ANALOG_PIN ) {
          adcDone = false;
          adcStarted = false;
          last_read = lread;
          lastPin = adcPin;

          ADMUX = bit (REFS0) | (analogPin[adcPin] & 0x07);
        }
        adcPin++;
      }
    }
  }
  
  // if we aren't taking a reading, start a new one
  if (!adcStarted) {
    adcStarted = true;
    // start the conversion
    if( !init_done ) {
      //Read internal Vref, to get accurate value with analogRead
      //http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
      //http://code.google.com/p/tinkerit/wiki/SecretVoltmeter

      // Read 1.1V reference against AVcc
      // set the reference to Vcc and the measurement to the internal 1.1V reference
      ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
      delay(2); // Wait for Vref to settle
    }
    ADCSRA |= bit (ADSC) | bit (ADIE);
  }
}


