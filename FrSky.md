
---

### FrSky telemetry ###
You can send telemetry data to your FrSky receiver, and then receive those informations to your Taranis radio or your FLD\_02 screen

Uncomment this, if you wish to send FrSky telemetry information (of course only working with FrSky telemetry module)
```
#define FRSKY_PROTOCOL
```

You can either display GPS information (for Taranis) in DMS (48°N52'13.782) or NMEA (48°52.1378N) information. Default is NMEA
```
//#define FRSKY_DMS
```

If your Taranis radio is displayed information in Imperials units, you should uncomment this line. Default is Metrics


Be carefull, with imperials, information are less accurate (number of satellites, eph, battery size), because of an internals conversions.
```
//#define FRSKY_IMPERIALS
```

Uncomment if FLD\_02 is used (leave commented for TARANIS)
```
//#define FRSKY_FLD02
```

## Send telemetry data ##

To send telemetry data, you have to connect arduino Pin 9 to RX pin of your FrSky telemetry receiver.
This was tested with Receiver D8R-XP and D8R-II plus, but it should work with any telemetry receiver.

![http://bagaosd.googlecode.com/svn/files/arduino_frsky.png](http://bagaosd.googlecode.com/svn/files/arduino_frsky.png)


### Please note ###

| **T2** |Will alternate display between GPS satellite / 3D fix information and a constant value (999) when home is not set. When home is set, only GPS satellite / 3D fix information are displayed.|
|:-------|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **T1** |Will only display battery size before flight. When copter is armed (throttle >10%) and flight time is above 2s, then display will alternate between battery size and eph                   |

**To change battery size** before flight, put the channel used to change MinimOSD pannel (**Pin7** for standard RX, **AUX** for PPM Sum mode receiver) in high position (>1800us), battery will change every second.

## Informations displayed ##

![http://bagaosd.googlecode.com/svn/files/taranris_01.jpg](http://bagaosd.googlecode.com/svn/files/taranris_01.jpg)

| **Parameter** | **Information** |
|:--------------|:----------------|
| **Vit**       | Speed information in Km/h (Metrics) or Knot (Imperials)|
| **Alt**       | Altitude above sea |
| **Carb**      | Battery remaining in percent |
| **Vfas**      | Battery voltage |
| **Rpm**       | Throttle PWM signal, stepped by 30, when copter is armed (throttle >10%)|
| **Cap**       | Compass heading information in degrees |
| **Cour**      | Current consumption |
| **T2**        | GPS satellite and 3D fix information. For exemple in 314 : 3 is 3D fix, and 14 is the number of satellites|
| **Cnsm**      | Standard information computed by Taranis (not sent from BagaOSD). Total amount of current consumed|
| **RSSI**      | Standard information computed by Taranis (not sent from BagaOSD). RSSI signal|
| **T1**        | GPS eph information. Quality GPS signal, not very good if above 220|
| **AltG**      | Altitude above ground. This information is set when copter is armed (throttle >10%) |


![http://bagaosd.googlecode.com/svn/files/taranris_02.jpg](http://bagaosd.googlecode.com/svn/files/taranris_02.jpg)

| **Parameter** | **Information** |
|:--------------|:----------------|
| Right side    | Different cells values. Not real cell values, but a division of global voltage per cell|


![http://bagaosd.googlecode.com/svn/files/taranris_gps.jpg](http://bagaosd.googlecode.com/svn/files/taranris_gps.jpg)

| **Parameter** | **Information** |
|:--------------|:----------------|
| **Latitude**  | Displayed either in DMS (48°N52'13.782) or NMEA (48°52.1378N)|
| **Longitude** | Displayed either in DMS (2°E14'33.71) or NMEA (2°14.3371E) |
