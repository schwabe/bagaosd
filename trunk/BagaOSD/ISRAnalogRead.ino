//VCC Internal
boolean init_done = false;

// ADC complete ISR
ISR (ADC_vect) {
    byte low, high;
    // we have to read ADCL first; doing so locks both ADCL
    // and ADCH until ADCH is read. reading ADCL second would
    // cause the results of each conversion to be discarded,
    // as ADCL and ADCH would be locked when it completed.
    low = ADCL;
    high = ADCH;

    if( !init_done) {
      uint16_t adcReading = low | (high << 8);

      intervalVCC = 1125300L / adcReading;  
      init_done = true;
    }
} // end of ADC_vect

void computeIntervalVCC() {
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  
  //enable ADC interupt : ISR (ADC_vect)
  ADCSRA |= bit(ADIE);
      
  //enable ADC
  ADCSRA |= bit(ADEN);
      
  //Start ADC
  ADCSRA |= bit(ADSC);
}
