For [BagaOSD\_V2.1.zip](http://bagaosd.googlecode.com/svn/build/BagaOSD_V2.1.zip) please change the following parameter
```
#define NAZA_COMPASS_TILT_COMP     FALSE
```

23.03.2014 : New update for BagaOSD\_V2.2.zip

### BagaOSD ###
| **Date** | **File** | **Size** | **Label** |
|:---------|:---------|:---------|:----------|
| 01.06.2014 | [BagaOSD\_V3.2.1.zip](http://bagaosd.googlecode.com/svn/build/BagaOSD_V3.2.1.zip)| 587 Ko   | Stable    |
| 29.05.2014 | [BagaOSD\_V3.1.zip](http://bagaosd.googlecode.com/svn/build/BagaOSD_V3.1.zip)| 594 Ko   | For testing|
| 08.05.2014 | [BagaOSD\_V3.0.zip](http://bagaosd.googlecode.com/svn/build/BagaOSD_V3.0.zip)| 774 Ko   | Beta      |
| 23.03.2014 | [BagaOSD\_V2.2.zip](http://bagaosd.googlecode.com/svn/build/BagaOSD_V2.2.zip)| 707 Ko   | Stable    |
| 14.02.2014 | [BagaOSD\_V2.1.zip](http://bagaosd.googlecode.com/svn/build/BagaOSD_V2.1.zip)| 705 Ko   | Beta      |
| 25.01.2014 | [BagaOSD\_V2.0.zip](http://bagaosd.googlecode.com/svn/build/BagaOSD_V2.0.zip)| 703 Ko   | Stable    |
| 06.11.2013 | [BagaOSD\_V1.0.zip](http://bagaosd.googlecode.com/svn/build/BagaOSD_V1.0.zip)| 753 Ko   | Stable    |


### MinimOSD Config tool ###
| **Date** | **File** | **Size** | **Label** |
|:---------|:---------|:---------|:----------|
| 16.03.2014 | [Config\_tool\_v2.4.0.0\_20140616.zip](http://bagaosd.googlecode.com/svn/build/Config_tool_v2.4.0.0_20140616.zip)|  539 Ko  | Stable    |
| 25.01.2014 | [Config\_tool\_v2.3.0.6\_20140125.zip](http://bagaosd.googlecode.com/svn/build/Config_tool_v2.3.0.6_20140125.zip)| 1.46 Mo  | Stable    |
| 06.11.2013 | [Config\_tool\_v2.1.2.0\_20131106.zip](http://bagaosd.googlecode.com/svn/build/Config_tool_v2.1.2.0_20131106.zip)|  505 Ko  | Stable    |

### MinimOSD ###
| **Date** | **File** | **Size** | **Label** |
|:---------|:---------|:---------|:----------|
| 31.05.2014 | [MinimOSD\_V2.4\_r736c.zip](http://bagaosd.googlecode.com/svn/build/MinimOSD_V2.4_r736c.zip)| 595Ko    | Stable    |
| 29.05.2014 | [MinimOSD\_V2.4\_r736b.zip](http://bagaosd.googlecode.com/svn/build/MinimOSD_V2.4_r736b.zip)| 602Ko    | Stable    |
| 16.03.2014 | [MinimOSD\_V2.4\_r736.zip](http://bagaosd.googlecode.com/svn/build/MinimOSD_V2.4_r736.zip)| 722 Ko   | Stable    |
| 25.01.2014 | [MinimOSD\_V2.4.zip](http://bagaosd.googlecode.com/svn/build/MinimOSD_V2.4.zip)| 719 Ko   | Stable    |
| 06.11.2013 | [MinimOSD\_V2.0.zip](http://bagaosd.googlecode.com/svn/build/MinimOSD_V2.0.zip)| 766 Ko   | Stable    |


### Config file ###
| **Date** | **File** | **Size** |
|:---------|:---------|:---------|
| 31.05.2014 | [config\_20140531.osd](http://bagaosd.googlecode.com/svn/build/config_20140531.osd) | 1.64 Ko  |
| 29.05.2014 | [config\_20140527.osd](http://bagaosd.googlecode.com/svn/build/config_20140527.osd) | 1.5 Ko   |


### Charset ###
| **Date** | **File** | **Size** |
|:---------|:---------|:---------|
| 29.05.2014 | [MinimOSD\_2.4.0.1.mcm](http://bagaosd.googlecode.com/svn/build/MinimOSD_2.4.0.1.mcm) | 160 Ko   |


### Release Notes ###
01.06.2014 - v3.2.1 ([r38](http://code.google.com/p/bagaosd/source/detail?r=38))
  * Major Bug fix from v3.1, the way analogread was done with ISR wasn't a good idea, back to classic analogread.
  * New parameter to set home altitude
  * Symbol HA in MinimOSD is blinking when home altitude is not set (same thing with telemetry, with T2)
  * Put some crontrol in MinimOSD to avoid too big numbers for Home altitude, ground speed, climb speed


29.05.2014 - v3.1 ([r29](http://code.google.com/p/bagaosd/source/detail?r=29))
  * Fix some few bugs (home altitude, better RX handling (less jerky))
  * Add the ability to change battery size before flight (FrSky telemetry)
  * T2 blink value when home is not set (FrSky telemetry)
  * T1 blink value between battery size and eph (FrSky telemetry)
  * Add an option to either compute heading from real heading (problem with tilt compensation), cog or a mixte of cog and real heading


08.05.2014 - v3.0 ([r24](http://code.google.com/p/bagaosd/source/detail?r=24))
  * FrSky telemetry added
  * RSSI PMW is not any more used. Instead use a RSSI Filter and connect it to an analog Pin
  * RSSI can be displayed as raw or percent values
  * RSSI value are buffered (to smooth output values)
  * Internal analog read are done with interrupt
  * Minor bugs fixed

16.03.2014 - v2.2 ([r21](http://code.google.com/p/bagaosd/source/detail?r=21))
  * Naza tilt compensation is default to FALSE (tilt compensation is experimental, so heading use COG instead of compass information)
  * Change gimbal default gain for better precision (set it to 20 in Naza GUI for Pitch/Roll)
  * Copter is armed when throttle is above 15%
  * Use last release ([r736](https://code.google.com/p/bagaosd/source/detail?r=736) - v2.4.0.0) of minimosd-extra
  * Change the way of computing flight time is minimosd-extra (flight time is counted when throttle is above 15%, and stop when bellow)
  * For RSSI, use Mavlink RSSI in minimosd-extra config tool

14.02.2014 - v2.1 (beta)
  * Ability to switch pannel in MinimOSD (ch8)
  * No more calibration for artificial horizon
  * Altitude bug corrected in MAVLINK\_MSG\_ID\_GPS\_RAW\_INT message
  * Add parameter for Naza gimbal output
  * Add parameter to tilt compensate Naza compass heading

25.01.2014 - v2.0 ([r8](http://code.google.com/p/bagaosd/source/detail?r=8)):
  * Change pin mapping (Pin 8 instead of TX) to output mavlink data to MinimOSD
  * Naza GPS Decoding information done with [NazaDecoderLib](http://www.rcgroups.com/forums/showthread.php?t=1995704) (thanks to pawelsky)
  * Add RSSIFilter to smooth RSSI PWM

06.11.2013 - v1.0 ([r1](http://code.google.com/p/bagaosd/source/detail?r=1)):
  * Initial release

