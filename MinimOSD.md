## What should I know about MinimOSD configuration ##

The [arduino part](HowTo.md) is only used to compute different sensors informations, those informations are displayed thru MinimOSD.
In order to display those informations, you should upload [minimosd-extra](http://code.google.com/p/minimosd-extra/) code to your MinimOSD device.

Please use the minimosd-extra [wiki](http://code.google.com/p/minimosd-extra/wiki/index), to get all information needed about how to upload code, how to configure, ...

BagaOSD come with a slighly modified version of minimosd-extra, BagaOSD use MavLINK 1.0 to comunicate with MinimOSD, so all upcoming version of minimosd-extra should be usable.


List of things modified in the minimosd-extra version coming with BagaOSD (modifications made from [r736](https://code.google.com/p/bagaosd/source/detail?r=736) / Copter: 2.4 stable release):

  * GPS HDOP horizontal (GPS quality signal)
  * Battery picture (6 positions)
  * Display RSSI with also a signal bar
  * **"atti"** mode is displayed instead of  **"alth"** (osd\_mode 2)
  * **"gpsa"** mode is displayed instead of  **"loit"** (osd\_mode 5)
  * **"fail"** mode is displayed instead of  **"retl"** (osd\_mode 6)
  * Change flight time computing

For RSSI, use Mavlink RSSI in minimosd-extra config tool
To display RSSI Signal bar, you should uncheck "RSSI Enable Raw" in the config panel of minimosd-extra config tool.

## OSD Toggle Channel ##
To use "OSD Toggle Channel" function of MinimOSD :
  * With a Standard receiver, choose "Ch 8" for "OSD Toggle Channel" and solder the receiver wire to Pin6 of Arduino.
  * With a PPM receiver, choose the correct channel in "OSD Toggle Channel" corresponding to your receiver channel.


## Informations send to MinimOSD ##
Several MavLINK message are sent to MinimOSD, at the following [rate](https://code.google.com/p/minimosd-extra/wiki/APM) :

|**Rate**|**Description**|**Message ID**|
|:-------|:--------------|:-------------|
|2hz     |waypoints, GPS raw, fence data, current waypoint, etc|[MAVLINK\_MSG\_ID\_GPS\_RAW\_INT](MinimOSD#MAVLINK_MSG_ID_GPS_RAW_INT.md)|
|5hz     |attitude and simulation state|[MAVLINK\_MSG\_ID\_ATTITUDE](MinimOSD#MAVLINK_MSG_ID_ATTITUDE.md)|
|2hz     |VFR\_Hud data  |[MAVLINK\_MSG\_ID\_VFR\_HUD](MinimOSD#MAVLINK_MSG_ID_VFR_HUD.md)|
|3hz     |AHRS, Hardware Status, Wind |[MAVLINK\_MSG\_ID\_SYS\_STATUS](MinimOSD#MAVLINK_MSG_ID_SYS_STATUS.md)|
|5hz     |radio input or radio output data|[MAVLINK\_MSG\_ID\_RC\_CHANNELS\_RAW](MinimOSD#MAVLINK_MSG_ID_RC_CHANNELS_RAW.md)|
|1hz     |heart beat     |MAVLINK\_MSG\_ID\_HEARTBEAT|

## Message description and mapping ##

### MAVLINK\_MSG\_ID\_GPS\_RAW\_INT ###

|**Variable**|**Description**|**Comment**|
|:-----------|:--------------|:----------|
|gps\_utc\_time\_second|[millis()](http://arduino.cc/en/Reference/millis)|Not real time but number of milliseconds since the Arduino board began running the current program|
|gpsFix      |0-1: no fix, 2: 2D fix, 3: 3D fix|GPS fix status|
|lat         | Latitude      | In 1E7 degrees|
|lon         | Longitude     | In 1E7 degrees|
|alt\_MSL    | Altitude in millimeters above MSL| Same as **alt** when decoding directly [Naza GPS](NazaConnector.md)|
|eph         | GPS HDOP horizontal| In cm     |
|epv         | GPS VDOP horizontal| In cm     |
|ground\_speed| GPS ground speed in cm/s| In cm     |
|cog         | Course over ground| In cd (degrees x 100) |
|numsats     | Number of satellites visible|           |


### MAVLINK\_MSG\_ID\_ATTITUDE ###

|**Variable**|**Description**|**Comment**|
|:-----------|:--------------|:----------|
|time\_boot\_ms|[millis()](http://arduino.cc/en/Reference/millis)|Not real time but number of milliseconds since the Arduino board began running the current program|
|roll        |Roll angle (rad)|Information come from Naza gimbal output|
|pitch       |Pitch angle (rad)|Information come from Naza gimbal output|
|yaw         |Yaw angle (rad)|Fixed value : 0|
|rollspeed   |Roll angular speed (rad/s)|Fixed value : 0|
|pitchspeed  |Pitch angular speed (rad/s)|Fixed value : 0|
|yawspeed    |Yaw angular speed (rad/s)|Fixed value : 0|


### MAVLINK\_MSG\_ID\_VFR\_HUD ###

|**Variable**|**Description**|**Comment**|
|:-----------|:--------------|:----------|
|airspeed    |Current airspeed in m/s|Same as groundspeed|
|groundspeed |Current ground speed in m/s|Information from GPS|
|heading     |Current heading in degrees|COG information. Using real heading only if decoding directly [Naza GPS](NazaConnector.md) with option NAZA\_COMPASS\_TILT\_COMP |
|throttle    |Current throttle setting in integer percent|0 to 100   |
|alt         |Current altitude (MSL)|In meters  |
|climb       |Current climb rate|In meters/second|


### MAVLINK\_MSG\_ID\_SYS\_STATUS ###

|**Variable**|**Description**|**Comment**|
|:-----------|:--------------|:----------|
|onboard\_control\_sensors\_present|showing which onboard controllers and sensors are present|Fixed value : 0|
|onboard\_control\_sensors\_enabled|showing which onboard controllers and sensors are enabled|Fixed value : 0|
|onboard\_control\_sensors\_health|showing which onboard controllers and sensors are operational|Fixed value : 0|
|load        |Maximum usage in percent of the mainloop time|Fixed value : 0|
|voltage\_battery|Battery voltage|In millivolts|
|current\_battery|Battery current|In 10 x milliamperes (1 = 10 milliampere), only used if there's a current sensor, otherwise it's a fixed value : 0|
|battery\_remaining|Remaining battery energy|(0%: 0, 100%: 100), used with current sensor and LIPO\_CAPACITY\_MAH to compute percent. Otherwise a function is used to compute percent from voltage|
|drop\_rate\_comm|Communication drops in percent|Fixed value : 0|
|errors\_comm|Communication errors|Fixed value : 0|
|errors\_count1|Autopilot-specific errors|Fixed value : 0|
|errors\_count2|Autopilot-specific errors|Fixed value : 0|
|errors\_count3|Autopilot-specific errors|Fixed value : 0|
|errors\_count4|Autopilot-specific errors|Fixed value : 0|


### MAVLINK\_MSG\_ID\_RC\_CHANNELS\_RAW ###

|**Variable**|**Description**|**Comment**|
|:-----------|:--------------|:----------|
|time\_boot\_ms|[millis()](http://arduino.cc/en/Reference/millis)|Milliseconds since system boot|
|port        |Servo output port|Fixed value : 0|
|chan1       |RC channel 1 value in microseconds|Gimball Roll value (Naza Output)|
|chan2       |RC channel 2 value in microseconds|Gimball Pitch value (Naza Output)|
|chan3       |RC channel 3 value in microseconds|Receiver Yaw in PPM or Fixed value in STD : 1500|
|chan4       |RC channel 4 value in microseconds|Receiver Throttle (PPM and STD)|
|chan5       |RC channel 5 value in microseconds|Receiver X1 in PPM or Fixed value in STD : 1500|
|chan6       |RC channel 6 value in microseconds|Receiver X2 in PPM or Fixed value in STD : 1500|
|chan7       |RC channel 7 value in microseconds|Receiver Flight Mode (PPM and STD)|
|chan8       |RC channel 8 value in microseconds|Receiver AUX (PPM and STD)|
|rssi        |Receive signal strength indicator|From 0 (0%) to 100 (100%)|
