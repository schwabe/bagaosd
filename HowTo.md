
---

# Introduction #

There are several steps to build your BagaOSD :
  * Make your communication cable
  * Either [solder](uBloxSolder.md) a wire on DJI Naza GPS or use a [connector](NazaConnector.md)
  * Solder every cables needed on Arduino
  * Upload Arduino code
  * Upload MinimOSD code, configuration and caracter
  * Test everything is working
  * Make a "compact" OSD with Shrink Tube and foam



---

# How to connect Arduino and MinimOSD #

Just glue 2 male to male servo lead together. You need a +5V wire (red), a ground wire (black) an one wire for communication (white), you can remove the other wire you don't need.

See picture below for the result of the connection cable

![http://bagaosd.googlecode.com/svn/files/cable_info.jpg](http://bagaosd.googlecode.com/svn/files/cable_info.jpg)

![http://bagaosd.googlecode.com/svn/files/minimosd-connection.jpg](http://bagaosd.googlecode.com/svn/files/minimosd-connection.jpg)


---

# How to get DJI Naza GPS information #

  * You can either [solder](uBloxSolder.md) a wire directely to the GPS to decode raw uBlox message
  * Or use a [connector](NazaConnector.md) to get DJI Naza message

Change the following parameter according to your configuration
```
#define DECODE_NAZA_GPS            
//Uncommented : Decode Naza GPS Protocol (connector choice)
//Commented: Decode RAW uBlox Message (solder choice)
```


---

# How to prepare your Arduino board #

There's 2 differents configuration, one using standard receiver configuration, the other one using receiver that output PPM.

All the [configuration](Parameter.md) is done in a single file "config.h". Of course you can change all the default values and the default PIN number assignment, **except receiver PIN assignment**


## Standard receiver configuration ##
| **PIN** | **Function** |
|:--------|:-------------|
| 2       | Throttle     |
| 4       | Gimbal roll for horizon display |
| 5       | Gimbal pitch for horizon display |
| 6       | MinimOSD Panel Switch |
| 7       | FMode (Flight mode : Acro / Atti / Gps / Failsafe) - Flight Mode. Used also to change battery size|
| 8       | Serial data to MinimOSD |
| 9       | Serial telemetry data to FrSky receiver (_`#define FRSKY_PROTOCOL`_) |

### Arduino with standard receiver ###
![http://bagaosd.googlecode.com/svn/files/arduino_std.png](http://bagaosd.googlecode.com/svn/files/arduino_std.png)



## PPM Sum receiver configuration ##
| **PIN** | **Function** |
|:--------|:-------------|
| 2       | PPM Sum signal from receiver |
| 4       | Gimbal roll for horizon display (it's not a PPM Sum signal)|
| 5       | Gimbal pitch for horizon display (it's not a PPM Sum signal)|
| 8       | Serial data to MinimOSD |
| 9       | Serial telemetry data to FrSky receiver (_`#define FRSKY_PROTOCOL`_) |

### Arduino with PPM Sum receiver ###
![http://bagaosd.googlecode.com/svn/files/arduino_ppm.png](http://bagaosd.googlecode.com/svn/files/arduino_ppm.png)


## RSSI Configuration ##
| **PIN** | **Function** |
|:--------|:-------------|
| A2      | Analog RSSI (_`#define RSSI_PIN_ANALOG`_) |


## Sensor ##
| **PIN** | **Function** |
|:--------|:-------------|
| A0      | Voltage sensor (_`#define VOLTAGE_PIN`_) |
| A3      | Current sensor (_`#define CURRENT_PIN`_)|
| RX      | GPS          |