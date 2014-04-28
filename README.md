MicroModem
==========

MicroModem is an educational implementation of a 1200-baud AFSK modem. This implementation is targeted at the Atmega 328p microprocessor used in many Arduino boards and spinoffs. It can be build very easily with an Arduino board based on the Atmega 328p, and around 15 common electronics components. I will add a Fritzing schematic as soon as I have time to draw it up.

I am currently writing an illustrated tutorial to go along with the source-code, but even now, the source code is extremely well documented, and all the concepts are explained in great detail in the comments.

By default MicroModem uses a very simple protocol dubbed MP1. The protocol enables end-to-end transport with forward error-correction and compression. Compression is courtesy of the excellent Heatshrink library. MP1 can also encapsulte TCP/IP traffic over SLIP.

While this project is based on Arduino hardware, it does not use the Arduino IDE. The project has been implemented in a normal C with makefile style, and uses some libraries from the open source BertOS.

Stay tuned for more :)
