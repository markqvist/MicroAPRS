MicroAPRS
==========

MicroAPRS is the APRS fork of [MicroModem](https://github.com/markqvist/MicroModem). It can be built very easily with an Arduino board based on the Atmega 328p, and around 15 common electronics components.

Please read the "Quickstart.pdf" in the Documentation folder for some pointers on building the modem!

Right now the APRS specific documentation is lacking, so all the docs included in this repository is directly from MicroModem, but it should still offer good pointers on building the modem, and getting started. The only difference is the firmware.

Currently a simple serial control protocol is implemented, while full KISS TNC functionality will be implemented soon.

## Modem control

Here's how to control the APRS modem over a serial connection. The modem accepts a variety of commands for setting options and sending packets. Generally a command starts with one or more characters defining the command, and then whatever data is needed to set the options for that command. Here's a list of the currently available commands:


##Serial commands

Command | Description
--- | :---
__!\<data>__  | Send raw packet
__@\<cmt>__ | Send location update (cmt = optional comment)
__#\<msg>__ | Send APRS message
&nbsp; | &nbsp;
__c\<call>__ |  Set your callsign
__d\<call>__ |  Set destination callsign
__1\<call>__ |  Set PATH1 callsign
__2\<call>__ |  Set PATH2 callsign
&nbsp; | &nbsp;
__sc\<ssid>__ | Set your SSID
__sd\<ssid>__ | Set destination SSID
__s1\<ssid>__ | Set PATH1 SSID
__s2\<ssid>__ | Set PATH2 SSID
&nbsp; | &nbsp;
__lla\<LAT>__ | Set latitude (NMEA-format, eg 4903.50N)
__llo\<LON>__ | Set latitude (NMEA-format, eg 07201.75W)
__lp\<0-9>__  | Set TX power info
__lh\<0-9>__  | Set antenna height info
__lg\<0-9>__  | Set antenna gain info
__ld\<0-9>__  | Set antenna directivity info
__ls\<sym>__  | Select symbol
__lt\<s/a>__  | Select symbol table (standard/alternate)
&nbsp; | &nbsp;
__mc\<call>__ | Set message recipient callsign
__ms\<ssid>__ | Set message recipient SSID
__mr\<ssid>__ | Retry last message
__ma\<1/0>__  | Automatic message ACK on/off
&nbsp; |&nbsp;
__ps\<1/0>__  | Print SRC on/off
__pd\<1/0>__  | Print DST on/off
__pp\<1/0>__  | Print PATH on/off
__pm\<1/0>__  | Print DATA on/off
__pi\<1/0>__  | Print INFO on/off
__v\<1/0>__ | Verbose mode on/off
__V\<1/0>__ | Silent mode on/off
&nbsp; | &nbsp;
__S__ | Save configuration
__L__ | Load configuration
__C__ | Clear configuration
__H__ | Print configuration



### Examples

#### To set your callsign to XX1YYY-5, send these two commands:
```
cXX1YYY
sc5
```

#### To send an APRS message to ZZ5ABC-0 with the content "Hi there!", using a raw packet, send this command:
```
!:ZZ5ABC-0 :Hi there!{01}
```
#### Here's an example of how to send a location update with power, height and gain information:
!=5530.70N/01143.70E-PHG2410Arduino MicroAPRS

### Serial connection

To connect to the modem use __9600 baud, 8N1__ serial. By default, the firmware uses time-sensitive input, which means that it will buffer serial data as it comes in, and when it has received no data for a few milliseconds, it will start interpreting whatever it has received. This means you need to set your serial terminal program to not send data for every keystroke, but only on new-line, or pressing send or whatever. If you do not want this behaviour, you can compile the firmware with the DEBUG flag set, which will make the modem wait for a new-line character before interpreting the received data. I would generally advise against this though, since it means that you cannot have newline characters in whatever data you want to send!

![MicroModem](https://raw.githubusercontent.com/markqvist/MicroModem/master/Design/Images/1.jpg)

The repository contains schematics and Eagle PCB files for a Microduino module. There is also fritzing sketches for both an Arduino shield and a Microduino module, with notes so they are easier to understand. I have had the eagle PCB fabricated, and it is working great, so you can use that if want to make your own board.

![MicroModem](https://raw.githubusercontent.com/markqvist/MicroModem/master/Design/Images/PCB-lo.png)

While this project is based on Arduino hardware, it does not use the Arduino IDE. The project has been implemented in your normal C with makefile style, and uses libraries from the open source BertOS.

Visit [my site](http://unsigned.io) for questions, comments and other details.
