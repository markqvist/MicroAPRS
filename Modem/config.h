
#ifndef CFG_AFSK_H
#define CFG_AFSK_H

/**
 * Module logging level.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_level"
 */
#define AFSK_LOG_LEVEL      LOG_LVL_WARN

/**
 * Module logging format.
 *
 * $WIZ$ type = "enum"
 * $WIZ$ value_list = "log_format"
 */
#define AFSK_LOG_FORMAT     LOG_FMT_TERSE


/**
 * AFSK discriminator filter type.
 *
 * $WIZ$ type = "enum"; value_list = "afsk_filter_list"
 */
#define CONFIG_AFSK_FILTER AFSK_CHEBYSHEV


/**
 * AFSK receiver buffer length.
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 2
 */
#define CONFIG_AFSK_RX_BUFLEN 64

/**
 * AFSK transimtter buffer length.
 *
 * $WIZ$ type = "int"
 * $WIZ$ min = 2
 */
#define CONFIG_AFSK_TX_BUFLEN 64

/**
 * AFSK DAC sample rate for modem outout.
 * $WIZ$ type = "int"
 * $WIZ$ min = 2400
 */
#define CONFIG_AFSK_DAC_SAMPLERATE 9600

/**
 * AFSK RX timeout in ms, set to -1 to disable.
 * $WIZ$ type = "int"
 * $WIZ$ min = -1
 */
#define CONFIG_AFSK_RXTIMEOUT 0


/**
 * AFSK Preamble length in [ms], before starting transmissions.
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 */
#define CONFIG_AFSK_PREAMBLE_LEN 300UL



/**
 * AFSK Trailer length in [ms], before stopping transmissions.
 * $WIZ$ type = "int"
 * $WIZ$ min = 1
 */
#define CONFIG_AFSK_TRAILER_LEN 50UL

#endif /* CFG_AFSK_H */
