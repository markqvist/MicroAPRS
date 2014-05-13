MicroModem
==========

MicroModem is an educational implementation of a 1200-baud AFSK modem. This implementation is targeted at the Atmega 328p microprocessor used in many Arduino boards and spinoffs. It can be build very easily with an Arduino board based on the Atmega 328p, and around 15 common electronics components.

![MicroModem](https://raw.githubusercontent.com/markqvist/MicroModem/master/Design/Images/1.jpg)

The repository contains schematics and Eagle PCB files for a Microduino module. There is also fritzing sketches for both an Arduino shield and a Microduino module, with notes so they are easier to understand. I have had the eagle PCB fabricated, and it is working great, so you can use that if want to make your own board.

I am currently writing an illustrated tutorial to go along with the source-code, but even now, the source code is extremely well documented, and all the concepts are explained in great detail in the comments.

By default MicroModem uses a very simple protocol dubbed MP1. The protocol enables end-to-end transport with forward error-correction and compression. Compression is courtesy of the excellent [Heatshrink](https://github.com/atomicobject/heatshrink) library. MP1 can also encapsulate TCP/IP traffic from a host computer with SLIP.

MicroModem was created as an easy to build, low-cost and very flexible radio-modem. It also supports the AX.25 implementation in BertOS, so it can be used with that for APRS and the likes very easily.

While this project is based on Arduino hardware, it does not use the Arduino IDE. The project has been implemented in your normal C with makefile style, and uses libraries from the open source BertOS.
