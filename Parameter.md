
---

# Modification for Arduino code #

## Compilation ##

Use arduino IDE (http://arduino.cc/en/main/software) to compile and upload your code to Arduino and MiniMOSD. You'll find a lot of tutorial about it on the internet.


---

## Config file ##

Modify file **config.h** according to your needs

---

### FrSky telemetry ###
You can send telemetry data to your FrSky receiver, and then receive those informations to your Taranis radio or your FLD\_02 screen

Uncomment this, if you wish to send FrSky telemetry information (of course only working with FrSky telemtry module)
```
#define FRSKY_PROTOCOL
```

You can either display GPS information (for Taranis) in DMS (48°N52'13.782) or NMEA (48°52.1378N) information. Default is NMEA
```
//#define FRSKY_DMS
```

If your Taranis radio is displayed information in Imperials units, you should uncomment this line. Default is Metrics
```
//#define FRSKY_IMPERIALS
```

Uncomment if FLD\_02 is used (leave commented for TARANIS)
```
//#define FRSKY_FLD02
```


---

### RSSI Choice (PWM or Analog) ###
#### PWM Rssi ####
If you're using a PWM rssi signal, you need to convert it to an analog signal, this can be done by a rssi filter.

![http://bagaosd.googlecode.com/svn/files/frsky_rssi_filter.jpg](http://bagaosd.googlecode.com/svn/files/frsky_rssi_filter.jpg)


#### Rssi parameters ####
Set the analog PIN (default A2) you want to use to read RSSI analog source.

If you don't need to display RSSI, assign -1 value (**#define RSSI\_PIN\_ANALOG -1**)
```
#define RSSI_PIN_ANALOG            A2
```

RSSI min read value. This value can either represent a RSSI min or max signal, it doesn't matter.
```
#define RSSI_MIN_VAL               350  
```

RSSI max read value. This value can either represent a RSSI min or max signal, it doesn't matter.
```
#define RSSI_MAX_VAL               670  
```

This value should represent the min signal value (RSSI\_MIN\_VAL or RSSI\_MAX\_VAL)
```
#define RSSI_VAL_LOW               RSSI_MIN_VAL 
```

Uncomment if you want to use raw RSSI value, instead of percent computed value
```
//#define RSSI_RAW_VAL                
```



---

### Radio receiver information ###

#### PPM / Standard ####
Uncomment if your receiver output PPM Sum (get all the channels information through one port only), this can be commented to use standard RX configuration instead of PPM Sum configuration.

```
#define RC_PPM_MODE                 
```

Set the channel order if you're using a PPM Sum receiver and **RC\_PPM\_MODE**.

Only 3 channels are used : **THROTTLE\_PPM** (for flight time and throttle percent) and **FMODE\_PPM** (for flight mode) and **AUX\_PPM** (Change MinimOSD panel / change battery size). But all the channels (THROTTLE / YAW / X1 / X2 / FMODE / AUX) are transmitted to MinimOSD


So you can use AUX channel to switch pannel inside MinimOSD (choose ch8)

```
#define SERIAL_SUM_PPM             ROLL_PPM,PITCH_PPM,THROTTLE_PPM,YAW_PPM,X1_PPM,X2_PPM,FMODE_PPM,AUX4_PPM,...
```



---

### Global sensor config ###

Used to have a better read accuracy for analog source.

Internal Arduino voltage can vary, so to get the same measured value, we should compare to a fixed reference. This can be commented to be disabled.

```
#define INTERNAL_VOLTAGE_REF       5160
```




---

### Voltage sensor config ###
Voltage sensor pin

```
#define VOLTAGE_PIN                A0
```

Rise this value to lower voltage displayed value

```
#define VOLTAGE_FACTOR             12.21
```

Internal buffer to smooth voltage value

```
#define VOLTAGE_BUFFER             128
```




---

### Current sensor config ###
Current sensor pin

```
#define CURRENT_PIN                A3
```

Rise this value to lower current output value

```
#define CURRENT_FACTOR             2.46
```

Internal buffer to smooth current value

```
#define CURRENT_BUFFER             128
```




---

### Battery config ###
Commented to use a current sensor, current sensor is not used if uncommented.
```
//#define ESTIMATE_BATTERY_REMAINING  ENABLED 
```

Number of cell, for estimate capacity (**ESTIMATE\_BATTERY\_REMAINING**)

```
#define LIPO_CELL                  4 
```

LIPO capacity in mah to dispaly remaining percent of battery (when a current sensor is used and **ESTIMATE\_BATTERY\_REMAINING** is disabled)

```
#define LIPO_CAPACITY_MAH          8000
```


LIPO capacity in mah can be change before flight. Change the battery size with the selector used to change MinimOSD panel (Channel plugged to **Pin7** for standard RX, or **AUX** for PPM Sum receiver)

```
#define LIPO_CAPACITY_MAH_MULTI    LIPO_CAPACITY_MAH,8000,10000  //LIPO capacity in mah selector
```


---

### GPS Handler ###
Choose what method to use to get Naza GPS data, uncommented for [Naza decoding](NazaConnector.md), commented for uBlow [raw decoding](uBloxSolder.md)

Uncommented : Decode Naza GPS Protocol (connector choice)

Commented : Decode RAW uBlox Message (GPS solder choice)
```
#define DECODE_NAZA_GPS              
```

When using Naza GPS Protocol, Heading (direct compass information) should be tilt compensate, this could be done when Naza Gimbal output is connected to BagaOSD.

Naza Gimbal output is not really accurate, so compass tilt compensation could lead to wrong values.

If you don't want real heading (NAZA\_COMPASS\_TILT\_COMP set to FALSE), COG will be used instead of heading.

You can also choose a mixte option (MIXTE) to get real heading when copter is not moving, and cog when copter is moving (>2m/s)
```
#define NAZA_COMPASS_TILT_COMP     FALSE  //TRUE or MIXTE
```


---

### Throttle limit ###
Those parameters are used to compute throttle percent. Put the min. and max. signal (leave default values if you don't know)
```
#define THROTTLE_PWM_MIN           1000
#define THROTTLE_PWM_MAX           2000
```


---

### Artificial horizon ###
The following paramters are used to adjust display of artificial horizon.

INVERT is used to invert the artificial horizon (uncomment according your needs)
```
#define PITCH_GAIN                 7.6 
#define PITCH_LEVEL                1500
//#define PITCH_INVERT               

#define ROLL_GAIN                  7.6 
#define ROLL_LEVEL                 1500
//#define ROLL_INVERT                
```

The following parameters were set in Naza for Gimbal output

**Servo travel limit**
|**Output**|**Max**|**Center**|**Min**|
|:---------|:------|:---------|:------|
|Pitch (F2)|1000   |-40       |-1000  |
|Roll (F1) |1000   |-30       |-1000  |

**Automatic Gain control**
|**Output**|**Gain**|**Direction**|
|:---------|:-------|:------------|
|Pitch     |20      |Normal       |
|Roll      |20      |Normal       |

**Manual Control Speed**
|**Output**|**Gain**|
|:---------|:-------|
|Pitch (X1)|0       |


---

### Home altitude ###
Define range precision in meter (20cm default) to stay in for 10s to set home altitude
```
#define HOME_SET_PRECISION         0.2
```

If home altitude couldn't be set with the previous method, after 90s, home altitude is set with the current altitude.
```
#define HOME_SET_AUTO_TIMEOUT      90
```