
#ifndef FSK_CFG
#define FSK_CFG

// Debug & test options
#define SERIAL_DEBUG false
#define PASSALL false
#define AUTOREPLY false

// Modem options
#define TX_MAXWAIT 2UL                      // How many milliseconds should pass with no
                                            // no incoming data before it is transmitted
#define CONFIG_AFSK_RX_BUFLEN 64            // The size of the modems receive buffer
#define CONFIG_AFSK_TX_BUFLEN 64            // The size of the modems transmit buffer
#define CONFIG_AFSK_DAC_SAMPLERATE 9600     // The samplerate of the DAC. Note that
                                            // changing it here will not change the
                                            // actual sample rate. It is defined here
                                            // so various functions can use it.
#define CONFIG_AFSK_RXTIMEOUT 0             // How long a read operation from the modem
                                            // will wait for data before timing out.

#define CONFIG_AFSK_PREAMBLE_LEN 350UL      // The length of the packet preamble in milliseconds
#define CONFIG_AFSK_TRAILER_LEN 50UL        // The length of the packet tail in milliseconds

#endif
