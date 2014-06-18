MicroAPRS
==========

MicroAPRS is the APRS fork of [MicroModem](https://github.com/markqvist/MicroModem). It can be build very easily with an Arduino board based on the Atmega 328p, and around 15 common electronics components.

Please read the "Quickstart.pdf" in the Documentation folder for some pointers on building the modem!

Right now the APRS specific documentation is lacking, so all the docs included in this repository is directly from MicroModem, but it should still offer good pointers on building the modem, and getting started. The only difference is the firmware, and I will include instructions for communicating with the modem over serial shortly.

Currently a simple serial control protocol is implemented, while full KISS TNC functionality will be implemented soon.

To connect to the modem use 9600 baud, 8N1 serial. By default, the firmware uses time-sensitive input, which means that it will buffer serial data as it comes in, and when it has received no data for a few milliseconds, it will start interpreting whatever it has received. This means you need to set your serial terminal program to not send data for every keystroke, but only on new-line, or pressing send or whatever. If you do not want this behaviour, you can compile the firmware with the DEBUG flag set, which will make the modem wait for a new-line character before interpreting the received data. I would generally advise against this though, since it means that you cannot have newline characters in whatever data you want to send!

![MicroModem](https://raw.githubusercontent.com/markqvist/MicroModem/master/Design/Images/1.jpg)

The repository contains schematics and Eagle PCB files for a Microduino module. There is also fritzing sketches for both an Arduino shield and a Microduino module, with notes so they are easier to understand. I have had the eagle PCB fabricated, and it is working great, so you can use that if want to make your own board.

![MicroModem](https://raw.githubusercontent.com/markqvist/MicroModem/master/Design/Images/PCB-lo.png)

While this project is based on Arduino hardware, it does not use the Arduino IDE. The project has been implemented in your normal C with makefile style, and uses libraries from the open source BertOS.

Visit [my site](http://unsigned.io) for questions, comments and other details.
