**BagaOSD** is a cheap way to have an OSD with DJI Naza. It runs on MinimOSD and Arduino hardware.
The name of BagaOSD comes from [Château de Bagatelle](http://en.wikipedia.org/wiki/Ch%C3%A2teau_de_Bagatelle), a place near Paris in France, where I fly.


Arduino hardware is used to compute sensors information and MinimOSD hardware is used to display the information computed by the Arduino. Communication between Arduino and MinimOSD is done with MavLINK 1.0


The original project of [MinimOSD](https://code.google.com/p/minimosd-extra) hardware was slighly [modified](MinimOSD.md) for BagaOSD, but communication is the same. You can use the original project if you want, but there's no guarantee it will work a 100%



The arduino code is based on differents projects :

  * RX decoding comes from [Multiwii](https://code.google.com/p/multiwii/)
  * GPS uBlox decoding from [Ardu-IMU](https://code.google.com/p/ardu-imu/)
  * GPS Naza decoding from [pawelsky](http://www.rcgroups.com/forums/showthread.php?t=1995704)
  * FrSky telemetry data from [Jani Hirvinen](https://github.com/evlas/jD-IOBoard)

This project was inspired from [FBOSD](http://firebug24k.com/index.php), a plug and play OSD version for DJI Naza.


What information are displayed with BagaOSD ?

With BagaOSD, you can display the following things :

  * Number of satellites
  * GPS Fix
  * GPS Coordinates
  * Satellite HDOP information (GPS quality signal)
  * Home distance
  * Absolute Altitude or Altitude above Home
  * Ground speed
  * Climb speed
  * Home direction (accurate information only when copter is moving)
  * Flight mode (ATTI / GPS / ACRO / FAILSAFE)
  * Throttle percent
  * Flight time
  * RSSI information (Analog or PWM)
  * Instant current consumption
  * Battery remaining (computed from voltage value or extract from current sensor)
  * Battery voltage
  * Artificial horizon
  * Send FrSky telemetry data


All the warning coming with MinimOSD extra are also available.


This project is GNU GPL v3 and using it is at your own risk.