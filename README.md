MicroModem
==========

MicroModem is an educational implementation of a 1200-baud AFSK modem. This implementation is targeted at the Atmega 328p microprocessor used in many Arduino boards and spinoffs. It can be build very easily with an Arduino board based on the Atmega 328p, and around 15 common electronics components.

The repository contains schematics and Eagle PCB files for an Microduino module. There is also fritzing sketches for both an Arduino shield and a Microduino module, with notes so they are easier to understand. The PCBs are currently untested, so don't trust them too much ;) I have just ordered a few samples and will assemble and test them as they come in. The schematics should be rather sound, they were drawn from the current working perfboard prototypes I have, but if you find an error, please let me know.

I am currently writing an illustrated tutorial to go along with the source-code, but even now, the source code is extremely well documented, and all the concepts are explained in great detail in the comments.

By default MicroModem uses a very simple protocol dubbed MP1. The protocol enables end-to-end transport with forward error-correction and compression. Compression is courtesy of the excellent Heatshrink library. MP1 can also encapsulate TCP/IP traffic from a host computer with SLIP.

MicroModem was created as an easy to build, low-cost and very flexible radio-modem. It also supports the AX.25 implementation in BertOS, so it can be used with that for APRS and the likes very easily.

While this project is based on Arduino hardware, it does not use the Arduino IDE. The project has been implemented in a normal C with makefile style, and uses some libraries from the open source BertOS.
