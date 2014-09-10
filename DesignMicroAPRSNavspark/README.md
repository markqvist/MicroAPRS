Standalone MicroAPRS + Navspark GPS
==========

Standalone MicroAPRS + Navspark GPS module is the is fork of [MicroAPRS](https://github.com/markqvist/MicroAPRS). It is a standalone arduino compatible board with modem circuitry acting as a daughter board for [NavSpark](http://navspark.com.tw/) GPS/Glonass/Beidu Arduino compatible modules. 

Please read the Mcicro APRS documentation prior to continuing here.

## Features

- Arduino Pro Mini 5V 16MHz compatible hardware design
- MicroAPRS modem design, with some component values tweaked
- Compatible with Wouxun/Baofeng low-cost radios that have a 3.5mm+2.5mm jack connection. UV-5R is a good choice.
- Powered either from USB via Navspark or from battery via Navspark on-board regulator
- Support for use of DS18B20 temperature sensors
- ADC channel with a divider for reading the battery voltage level

![MicroAPRSNavspark](https://raw.githubusercontent.com/IRNAS/MicroAPRS/master/DesignMicroAPRSNavspark/photos/navspark-microaprs-1.jpg)

## Instructions

Get the PCB manufactured with your preferred source, gerber files are published here. Solder the circuit and upload MicroAPRS code. Program Navspark with the code published here, modify it to contain you callsign and SSID. Enjoy.

By default location updates will be sent out ever 60s when then location fix is obtained.

### ToDo

Currently this this tracking system is very simple and sends a location update at a fixed interval. With NavSpark library it is possible to get the distance and many other parameters, creating a smarter location update mechanism.

The code can be cleaned up and optimized. 

Create a 3D printed enclosure that clips to a radio and powers the circuit from its charging contacts.

Optimize the Navspark Arduino applicaton for power consumption.


![MicroAPRSNavspark](https://raw.githubusercontent.com/IRNAS/MicroAPRS/master/DesignMicroAPRSNavspark/photos/navspark-microaprs-2.jpg)
![MicroAPRSNavspark](https://raw.githubusercontent.com/IRNAS/MicroAPRS/master/DesignMicroAPRSNavspark/photos/navspark-microaprs-3.jpg)
![MicroAPRSNavspark](https://raw.githubusercontent.com/IRNAS/MicroAPRS/master/DesignMicroAPRSNavspark/photos/navspark-microaprs-4.jpg)
