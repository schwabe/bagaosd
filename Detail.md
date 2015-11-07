
---

# Common configuration #
Common configuration between PPM Sum and standard receiver (_see [build](HowTo.md) part for wiring_).


## DJI Naza GPS ##

_**If you need to display the following things :**_

  * Number of satellites
  * GPS Fix
  * Satellite HDOP information (GPS quality signal)
  * Home distance
  * Absolute Altitude or Altitude above Home
  * Ground speed
  * Home direction (accurate information only when copter is moving)

You have to connect the DJI Naza GPS to the **pin 8** of the Arduino.

You have two options to get GPS Naza data :

  1. [Solder](uBloxSolder.md) a wire directely on the GPS Naza
  1. Build an [adapter](NazaConnector.md) to directely get GPS Naza information, and keep Naza GPS connected to Naza Main Controller


## DJI Naza gimbal output ##

_**If you need to display an artificial horizon**_

If you don't use any gimbal connected to your DJI Naza, and you want to display an artificial horizon, you can then use the gimbal output of DJI Naza for this.
  * Gimbal Roll  (Pin 4) : For roll artificial horizon information
  * Gimbal Pitch (Pin 5) : For pitch artificial horizon information

You may have to adjust the gimbal gain of your DJI Naza, to have a proper display.

For exemple you can set _pitch gain to 12 and roll gain to 8_


## Current Sensor ##

_**If you need to display the following things :**_

  * Instant current consumption
  * Real battery remaining

Connect current sensor to pin A3, if you want to display current information.

**!!! Please notice, that current sensor output should not be above 5V**

If you're flying with a [TBS Discovery Frame](http://team-blacksheep.com) you can use the current sensor coming in standard with this frame.
The parameter configuration is
```
#define CURRENT_FACTOR             2.05  
```


## Voltage Sensor ##

_**If you need to display the following things :**_

  * Battery voltage
  * Estimate battery remaining

Connect voltage sensor to pin A0, if you want to display voltage information.

If you don't have any voltage sensor, you can build it with resistors ([Voltage divider](http://en.wikipedia.org/wiki/Voltage_divider)) or you can use a cheap one, for exemple the FrSky Battery Voltage Sensor.

**!!! Please notice, that voltage information should not be above than 5V**

## RSSI ##

RSSI information can be displayed from an analog source or a PWM source (like FrSky receiver)

If your source is a PWM source, than you should build a RSSI filter with a resistor and a capacitor before connecting to an arduino analog pin.

![http://bagaosd.googlecode.com/svn/files/frsky_rssi_filter.jpg](http://bagaosd.googlecode.com/svn/files/frsky_rssi_filter.jpg)

Connect your RSSI source to PIN A2 (_**be careful this source shouldn't provide voltage above 5V**_).



---

# Specific configuration #

If you need to display the following things :

  * Flight mode (ATTI / GPS / ACRO / FAILSAFE)
  * Throttle percent
  * Flight time


## Standard receiver ##

Connect receiver throttle wire to PIN 2 (Throttle percent / Flight time) and flight mode wire (normally this information come from receiver channel 5) to PIN 7


## PPM receiver ##

Connect receiver PPM wire to PIN 2


_**Please note that PIN assignment can't be change for Standard receiver or PPM receiver.**_