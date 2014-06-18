#ifndef PROTOCOL_SIMPLE_SERIAL
#include <net/ax25.h>
#include <drv/ser.h>

#define PROTOCOL_SIMPLE_SERIAL 0x01

#define DEFAULT_CALLSIGN "NOCALL"
#define DEFAULT_DESTINATION_CALL "APZMDM"

void ss_messageCallback(struct AX25Msg *msg, Serial *ser);
void ss_serialCallback(void *_buffer, size_t length, Serial *ser, AX25Ctx *ctx);
void ss_printSrc(bool val);
void ss_printDst(bool val);
void ss_printPath(bool val);
void ss_printData(bool val);
void ss_printInfo(bool val);

void ss_sendMsg(void *_buffer, size_t length, AX25Ctx *ax25);

#endif