# Elements needed to build a Naza GPS Connector #

This connector will be used to transmit serial Naza GPS data directly to BagaOSD, and also keep Naza GPS communicating with Naza Main Controller

Please be carefull with wiring, otherwise you can fry your device.


## GPS plug description ##

  * Be carrefull GPS plug connector rib are always face down
  * Data will be read from TX pin of the GPS plug

![http://bagaosd.googlecode.com/svn/files/naza_gps_connector.jpg](http://bagaosd.googlecode.com/svn/files/naza_gps_connector.jpg)



## How to build your own connector ##

This adaptor is made in 3 parts :
  1. One _**Female Header Socket**_ 4 pin long to connect adaptor to the Naza MC (Main Controller)
  1. One _**Break Away Headers - Straight**_ 4 pin long to connect the Naza GPS to the adaptor
  1. One wire to send serial GPS Data to BagaOSD

![http://bagaosd.googlecode.com/svn/files/naza_gps_adaptor_part1.jpg](http://bagaosd.googlecode.com/svn/files/naza_gps_adaptor_part1.jpg)



## Build your connector ##

  1. Solder the 4 pin of the _**Break Away Headers - Straight**_ with the 4 pin of the _**Female Header Socket**_
  1. Solder a wire on the second pin of your adapter.

In my case, I didn' solder any ground wire, because everything is powered from a single battery.
If your Naza MC and your BagaOSD are not powered from the same battery, you need to solder also a ground wire.

![http://bagaosd.googlecode.com/svn/files/naza_gps_adaptor_part2.jpg](http://bagaosd.googlecode.com/svn/files/naza_gps_adaptor_part2.jpg)



After testing you don't have any shortcut between pin, and your wire is well connected, insulate the 4 contacts with whatever you want (expoxy, heatshrink, Instamorph Pellets, ...).
I put a white mark on the top of my connector, to avoid any confusion when I plug it to the Naza MC.

![http://bagaosd.googlecode.com/svn/files/naza_gps_adaptor_part3.jpg](http://bagaosd.googlecode.com/svn/files/naza_gps_adaptor_part3.jpg)


## How to use it ##
**Plug the adaptor into the MC instead of the GPS**

  * Ground (GND) of MC is in front of **X3 label**
  * Receive pin (RX) of MC (connected to TX of GPS) is in front of **X2 label**
  * Vcc (+5V) of MC is in front of **U label**

If your connector doesn't fit very well, you can use a cutter and remove some materiel on the both side of the connector. Do it carrefuly, don't remove to much material.

![http://bagaosd.googlecode.com/svn/files/naza_gps_adaptor_plug_part1.jpg](http://bagaosd.googlecode.com/svn/files/naza_gps_adaptor_plug_part1.jpg)




**Plug the GPS on the adaptor**

You souldn't see the GPS connector rib

![http://bagaosd.googlecode.com/svn/files/naza_gps_adaptor_plug_part2.jpg](http://bagaosd.googlecode.com/svn/files/naza_gps_adaptor_plug_part2.jpg)



After doing this, please verify the adaptor and GPS are well attached, they are not moving away. To be sure, you can put a piece of tape, to keep adaptor and GPS in position.


