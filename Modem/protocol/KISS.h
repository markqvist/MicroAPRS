#ifndef _PROTOCOL_KISS
#define _PROTOCOL_KISS 0x02

#define FEND 0xC0
#define FESC 0xDB
#define TFEND 0xDC
#define TFESC 0xDD

#define CMD_UNKNOWN 0xFE
#define CMD_DATA 0x00
#define CMD_TXDELAY 0x01
#define CMD_P 0x02
#define CMD_SLOTTIME 0x03
#define CMD_TXTAIL 0x04
#define CMD_FULLDUPLEX 0x05
#define CMD_SETHARDWARE 0x06
#define CMD_RETURN 0xFF

#include <net/ax25.h>
#include <drv/ser.h>

void kiss_init(AX25Ctx *ax25, Serial *ser);

void kiss_messageCallback(AX25Ctx *ctx);
void kiss_serialCallback(uint8_t sbyte);
void fon(void);
#endif