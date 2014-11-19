
//////////////////////////////////////////////////////
// First things first, all the includes we need     //
//////////////////////////////////////////////////////

#include <cpu/irq.h>        // Interrupt functionality from BertOS

#include <drv/ser.h>        // Serial driver from BertOS
#include <drv/timer.h>      // Timer driver from BertOS

#include <stdio.h>          // Standard input/output
#include <string.h>         // String operations
#define F_CPU 16000000UL
#include <util/delay.h>

#include <net/ax25.h>

#include "afsk.h"           // Header for AFSK modem


#include "protocol/SimpleSerial.h"   // Simple serial control protocol
#include "protocol/KISS.h"           // KISS TNC protocol

#if SERIAL_DEBUG
    #include "cfg/debug.h"  // Debug configuration from BertOS
#endif

//////////////////////////////////////////////////////
// A few definitions                                //
//////////////////////////////////////////////////////

static Afsk afsk;           // Declare a AFSK modem struct
static AX25Ctx ax25;        // Declare a protocol struct
static Serial ser;          // Declare a serial interface struct

#define ADC_CH 0            // Define which channel (pin) we want
                            // for the ADC (this is A0 on arduino)

#if SERIAL_PROTOCOL == PROTOCOL_SIMPLE_SERIAL
    static uint8_t serialBuffer[CONFIG_AX25_FRAME_BUF_LEN+1]; // Buffer for holding incoming serial data
    static int sbyte;            // For holding byte read from serial port
    static size_t serialLen = 0;                              // Counter for counting length of data from serial
    static bool sertx = false;                                // Flag signifying whether it's time to send data
                                                              // received on the serial port.
#endif

#if SERIAL_PROTOCOL == PROTOCOL_KISS
    static uint8_t sbyte;            // For holding byte read from serial port
#endif

#define SER_BUFFER_FULL (serialLen < CONFIG_AX25_FRAME_BUF_LEN-1)


//////////////////////////////////////////////////////
// And here comes the actual program :)             //
//////////////////////////////////////////////////////

// This is a callback we register with the protocol,
// so we can process each packet as they are decoded.
// Right now it just prints the packet to the serial port.
#if SERIAL_PROTOCOL == PROTOCOL_SIMPLE_SERIAL
static void message_callback(struct AX25Msg *msg)
{
    ss_messageCallback(msg, &ser);
}
#endif

#if SERIAL_PROTOCOL == PROTOCOL_KISS
static void message_callback(struct AX25Ctx *ctx)
{
    kiss_messageCallback(ctx);
}
#endif

// Simple initialization function.
static void init(void)
{
    // Enable interrupts
    IRQ_ENABLE;

    // Initialize hardware timers
    timer_init();

    // Initialize serial comms on UART0,
    // which is the hardware serial on arduino
    ser_init(&ser, SER_UART0);
    ser_setbaudrate(&ser, 9600);

    // For some reason BertOS sets the serial
    // to 7 bit characters by default. We set
    // it to 8 instead.
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

    // Create a modem context
    afsk_init(&afsk, ADC_CH);
    // ... and a protocol context with the modem
    ax25_init(&ax25, &afsk.fd, message_callback);

    #if SERIAL_PROTOCOL == PROTOCOL_SIMPLE_SERIAL
        // Init SimpleSerial
        ss_init(&ax25);
    #endif
    
    #if SERIAL_PROTOCOL == PROTOCOL_KISS
        // Init KISS
        kiss_init(&ax25, &afsk, &ser);
    #endif
    
    // That's all!
}

int main(void)
{
    // Start by running the main initialization
    init();
    
    #if SERIAL_PROTOCOL == PROTOCOL_KISS
        while (1) {
            // First we instruct the protocol to check for
            // incoming data
            ax25_poll(&ax25);

            if (ser_available(&ser)) {
                sbyte = ser_getchar_nowait(&ser);
                kiss_serialCallback(sbyte);
            }

        }
    #endif

    #if SERIAL_PROTOCOL == PROTOCOL_SIMPLE_SERIAL
        // Record the current tick count for time-keeping
        ticks_t start = timer_clock();
        // Go into ye good ol' infinite loop
        while (1) {    
            // First we instruct the protocol to check for
            // incoming data
            ax25_poll(&ax25);

            // Poll for incoming serial data
            if (!sertx && ser_available(&ser)) {
                // We then read a byte from the serial port.
                // Notice that we use "_nowait" since we can't
                // have this blocking execution until a byte
                // comes in.
                sbyte = ser_getchar_nowait(&ser);

                // If SERIAL_DEBUG is specified we'll handle
                // serial data as direct human input and only
                // transmit when we get a LF character
                #if SERIAL_DEBUG
                    // If we have not yet surpassed the maximum frame length
                    // and the byte is not a "transmit" (newline) character,
                    // we should store it for transmission.
                    if ((serialLen < CONFIG_AX25_FRAME_BUF_LEN) && (sbyte != 10)) {
                        // Put the read byte into the buffer;
                        serialBuffer[serialLen] = sbyte;
                        // Increment the read length counter
                        serialLen++;
                    } else {
                        // If one of the above conditions were actually the
                        // case, it means we have to transmit, se we set
                        // transmission flag to true.
                        sertx = true;
                    }
                #else
                    // Otherwise we assume the modem is running
                    // in automated mode, and we push out data
                    // as it becomes available. We either transmit
                    // immediately when the max frame length has
                    // been reached, or when we get no input for
                    // a certain amount of time.

                    if (serialLen < CONFIG_AX25_FRAME_BUF_LEN-1) {
                        // Put the read byte into the buffer;
                        serialBuffer[serialLen] = sbyte;
                        // Increment the read length counter
                        serialLen++;
                    } else {
                        // If max frame length has been reached
                        // we need to transmit.
                        serialBuffer[serialLen] = sbyte;
                        serialLen++;
                        sertx = true;
                    }

                    start = timer_clock();
                #endif
            } else {
                if (!SERIAL_DEBUG && serialLen > 0 && timer_clock() - start > ms_to_ticks(TX_MAXWAIT)) {
                    sertx = true;
                }
            }

            if (sertx) {
                ss_serialCallback(serialBuffer, serialLen, &ser, &ax25);
                sertx = false;
                serialLen = 0;
            }

        }
    #endif
    
    return 0;
}