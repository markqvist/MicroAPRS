
//////////////////////////////////////////////////////
// First things first, all the includes we need     //
//////////////////////////////////////////////////////

#include <cpu/irq.h>        // Interrupt functionality from BertOS

#include <drv/ser.h>        // Serial driver from BertOS
#include <drv/timer.h>      // Timer driver from BertOS

#include <stdio.h>          // Standard input/output
#include <string.h>         // String operations

#include <net/ax25.h>

#include "afsk.h"           // Header for AFSK modem

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

#define YOUR_CALLSIGN "nocall"
#define TO_CALL "apzmdm"
static AX25Call path[] = AX25_PATH(AX25_CALL(TO_CALL, 0), AX25_CALL(YOUR_CALLSIGN, 0), AX25_CALL("wide1", 1), AX25_CALL("wide2", 2));
#define SEND_TEST_PACKETS true
#define TEST_INTERVAL 15000L
#define APRS_MSG    "Test APRS packet"



#define SER_BUFFER_FULL (serialLen < MP1_MAX_DATA_SIZE-1)

//////////////////////////////////////////////////////
// And here comes the actual program :)             //
//////////////////////////////////////////////////////

// This is a callback we register with the protocol,
// so we can process each packet as they are decoded.
// Right now it just prints the packet to the serial port.
static void message_callback(struct AX25Msg *msg)
{
    kfile_printf(&ser.fd, "\n\nSRC[%.6s-%d], DST[%.6s-%d]\r\n", msg->src.call, msg->src.ssid, msg->dst.call, msg->dst.ssid);

    for (int i = 0; i < msg->rpt_cnt; i++)
        kfile_printf(&ser.fd, "via: [%.6s-%d]\r\n", msg->rpt_lst[i].call, msg->rpt_lst[i].ssid);

    kfile_printf(&ser.fd, "DATA: %.*s\r\n", msg->len, msg->info);
}

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

    // That's all!
}

int main(void)
{
    // Start by running the main initialization
    init();
    // Record the current tick count for time-keeping
    ticks_t start = timer_clock();
    
    // Go into ye good ol' infinite loop
    while (1)
    {    
        // First we instruct the protocol to check for
        // incoming data
        ax25_poll(&ax25);    

        // Use AX.25 to send test data
        if (SEND_TEST_PACKETS && timer_clock() - start > ms_to_ticks(TEST_INTERVAL))
        {
            start = timer_clock();
            ax25_sendVia(&ax25, path, countof(path), APRS_MSG, sizeof(APRS_MSG));
        }

    }
    return 0;
}