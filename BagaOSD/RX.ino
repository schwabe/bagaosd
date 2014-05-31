#define MEDIAN_PWM_SIZE  7  //5->44us 7->84us 9->128us 11->188us

volatile uint16_t rcValueSTD[RC_CHANS_STD] = {1502, 1502, 1502, 1502, 1502}; // interval [1000;2000]
const uint8_t PCInt_RX_Pins[PCINT_PIN_COUNT] = {PCINT_RX_BITS}; //pin 2 / 4 / 5 / 6 / 7 // if this slowes the PCINT readings we can switch to a define for each pcint bit

volatile uint16_t rcValuePPM[RC_CHANS_PPM] = {1502, 1502, 1502, 1502, 1502, 1502, 1502, 1502, 1502, 1502, 1502, 1502}; // interval [1000;2000]

const uint8_t rcChannelPPM[RC_CHANS_PPM] = {SERIAL_SUM_PPM};

RunningUintAverage pwmMedian(MEDIAN_PWM_SIZE); 

unsigned long lastcomputetime=0;

/* Pin to interrupt map:
 * D0-D7 = PCINT 16-23 = PCIR2 = PD = PCIE2 = pcmsk2
 * D8-D13 = PCINT 0-5 = PCIR0 = PB = PCIE0 = pcmsk0
 * A0-A5 (D14-D19) = PCINT 8-13 = PCIR1 = PC = PCIE1 = pcmsk1
 */
void configureReceiver() {
    // PCINT activation
    for(uint8_t i = 0; i < PCINT_PIN_COUNT; i++){ // i think a for loop is ok for the init.
      PCINT_RX_PORT |= PCInt_RX_Pins[i]; //Configure Input pin / Output pin (1 for Ouptut, 0 for Input)
      PCINT_RX_MASK |= PCInt_RX_Pins[i]; //Enable watch change value for pin in Input state (same as above)
    }
    PCICR = PCIR_PORT_BIT;  //Enable interrupt for watching change values
}

//Mask: changing pin
//Pin : state of all the pin
#define RX_PIN_CHECK(pin_pos)                        \
if (mask & PCInt_RX_Pins[pin_pos]) {                             \
  if (!(pin & PCInt_RX_Pins[pin_pos])) {                         \
    dTime = cTime-edgeTime[pin_pos];                             \
    if (900<dTime && dTime<2200) {                               \
      rcValueSTD[pin_pos] = dTime;                             \
    }                                                            \
  } else edgeTime[pin_pos] = cTime;                              \
}
    
// port change Interrupt
ISR(RX_PC_INTERRUPT) { //this ISR is common to every receiver channel, it is call everytime a change state occurs on a RX input pin
  uint8_t mask;
  uint8_t pin;
  uint16_t cTime,dTime;
  static uint16_t edgeTime[RC_CHANS_STD]; //5
  static uint8_t PCintLast;

  pin = RX_PCINT_PIN_PORT; // RX_PCINT_PIN_PORT indicates the state of each PIN for the arduino port dealing with Ports digital pins
 
  mask = pin ^ PCintLast;          // doing a ^ between the current interruption and the last one indicates wich pin changed
  cTime = micros(); //timer2.get_T2_umicros(); //return a uint32_t, but it is not usefull to keep the whole bits => we keep only 16 bits
  sei();                           // re enable other interrupts at this point, the rest of this interrupt is not so time critical and can be interrupted safely
  PCintLast = pin;                 // we memorize the current state of all PINs [D0-D7]

    #if (PCINT_PIN_COUNT > 0) //THROTTLEPIN => Throttle or PPM Signal
      #if defined(RC_PPM_MODE)
        if ((mask & PCInt_RX_Pins[THROTTLE_STD]) &&  //Compute PPM, when PPM pin state change
            (pin & PCInt_RX_Pins[THROTTLE_STD])) {   //and when PPM pin is high => both say it's rising 
            computePPM(cTime);
        }
      #else 
        RX_PIN_CHECK(THROTTLE_STD);
      #endif
    #endif
   
    #if (PCINT_PIN_COUNT > 1) //ROLLPIN
      RX_PIN_CHECK(GIMBALROLL_STD);
    #endif
    
    #if (PCINT_PIN_COUNT > 2) //PITCHPIN
      RX_PIN_CHECK(GIMBALPITCH_STD);
    #endif
    
    #if (PCINT_PIN_COUNT > 3) //AUXPIN
      RX_PIN_CHECK(AUX_STD);
    #endif
    
    #if (PCINT_PIN_COUNT > 4) //FMODEPIN
      RX_PIN_CHECK(FMODE_STD);
    #endif
}


uint16_t readRawRC(uint8_t chan) {
  uint16_t data;
  uint8_t oldSREG;
  oldSREG = SREG; cli(); // Copy interrupt state (don't know state) and then to be safe let's disable interrupts (maybe already disabled or not)

  data = rcValueSTD[chan]; // Let's copy the data Atomically
  
  SREG = oldSREG;        // Let's restore interrupt state

  return data; // We return the value correctly copied when the IRQ's where disabled
}

void computePPM(uint16_t now) {
    uint16_t diff;
    static uint16_t last = 0;
    static uint8_t chan = 0;
  
    diff = now - last;
    last = now;
    if(diff>3000) {
      #if defined(DEBUG_LOOP)
        cppm_loop = chan;
      #endif
        chan = 0;
    } else {
        if(900<diff && diff<2200 && chan<RC_CHANS_PPM ) {   //Only if the signal is between these values it is valid, otherwise the failsafe counter should move up
            rcValuePPM[chan] = diff;
        }
        chan++;
    }
}

uint16_t readPPMRawRC(uint8_t chan) {
  uint16_t data;
  uint8_t oldSREG;
  oldSREG = SREG; cli(); // Copy interrupt state (don't know state) and then to be safe let's disable interrupts (maybe already disabled or not)
  
  data = rcValuePPM[rcChannelPPM[chan]]; // Let's copy the data Atomically
  
  SREG = oldSREG;        // Let's restore interrupt state

  return data; // We return the value correctly copied when the IRQ's where disabled
}

/**************************************************************************************/
/***************          compute and Filter the RX data           ********************/
/**************************************************************************************/

void computeRC_STD() {
  static uint16_t rcData4Values[RC_CHANS_STD][MEDIAN_PWM_SIZE], rcDataMean[RC_CHANS_STD];
  static uint8_t rc4ValuesIndex = 0; //Average on 4 differents values
  uint8_t chan,a;
  
  rc4ValuesIndex++;
  if (rc4ValuesIndex == MEDIAN_PWM_SIZE) rc4ValuesIndex = 0; //need to average because micros() as 4us precision
  for (chan = 0; chan < RC_CHANS_STD; chan++) {
    rcData4Values[chan][rc4ValuesIndex] = readRawRC(chan);
    for (a=0;a<MEDIAN_PWM_SIZE;a++) {
      pwmMedian.addValue(rcData4Values[chan][a]);
    }
    //Filter extremes PMW values
    rcDataSTD[chan] = pwmMedian.getMedian();
  }
}


void computeRC_PPM() {
  static uint16_t rcData4ValuesPPM[RC_CHANS_PPM][MEDIAN_PWM_SIZE], rcDataMeanPPM[RC_CHANS_PPM];
  static uint16_t rcData4Values[RC_CHANS_STD][MEDIAN_PWM_SIZE], rcDataMean[RC_CHANS_STD];
  static uint8_t rc4ValuesIndexPPM = 0;
  uint8_t chan,a;
  
  rc4ValuesIndexPPM++;
  //PPM Sum output
  if (rc4ValuesIndexPPM == MEDIAN_PWM_SIZE) rc4ValuesIndexPPM = 0; //need to average because micros() as 4us precision
  for (chan = 0; chan < 8; chan++) { //Only 8 channels are used for PPM
    rcData4ValuesPPM[chan][rc4ValuesIndexPPM] = readPPMRawRC(chan);
    for (a=0;a<MEDIAN_PWM_SIZE;a++) {
      pwmMedian.addValue(rcData4ValuesPPM[chan][a]);
    }
    //Filter extremes PMW values
    rcDataPPM[chan] = pwmMedian.getMedian();
  }
  
  //Gimbal output (PWM)
  for (chan = 0; chan < RC_CHANS_STD; chan++) {
    if( chan == GIMBALROLL_STD || chan == GIMBALPITCH_STD) {
      rcData4Values[chan][rc4ValuesIndexPPM] = readRawRC(chan); 
      for (a=0;a<MEDIAN_PWM_SIZE;a++) {
        pwmMedian.addValue(rcData4Values[chan][a]);
      }
      //Filter extremes PMW values
      rcDataSTD[chan] = pwmMedian.getMedian();
    }  
  }
  
  rcDataSTD[THROTTLE_STD] = rcDataPPM[THROTTLE_PPM];
  rcDataSTD[FMODE_STD] = rcDataPPM[FMODE_PPM];
  rcDataSTD[AUX_STD] = rcDataPPM[AUX4_PPM];
}


void analyseRC() {
    #if defined(RC_PPM_MODE)
        computeRC_PPM(); //Decode PPM Sum
    #else
        computeRC_STD(); //Read PWM from RX
    #endif
    
    throttle_pwm = rcDataSTD[THROTTLE_STD];
    roll_pwm = rcDataSTD[GIMBALROLL_STD];
    pitch_pwm = rcDataSTD[GIMBALPITCH_STD];
    panel_pwm = rcDataSTD[AUX_STD];
}
