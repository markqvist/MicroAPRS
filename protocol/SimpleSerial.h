#ifndef _PROTOCOL_SIMPLE_SERIAL
#define _PROTOCOL_SIMPLE_SERIAL 0x01
#include "AX25.h"

#define DEFAULT_CALLSIGN "NOCALL"
#define DEFAULT_DESTINATION_CALL "APZMDM"

void ss_init(AX25Ctx *ax25);

void ss_messageCallback(struct AX25Msg *msg);
void ss_serialCallback(void *_buffer, size_t length, AX25Ctx *ctx);

void ss_sendPkt(void *_buffer, size_t length, AX25Ctx *ax25);
void ss_sendLoc(void *_buffer, size_t length, AX25Ctx *ax25);
void ss_sendMsg(void *_buffer, size_t length, AX25Ctx *ax25);
void ss_msgRetry(AX25Ctx *ax25);

void ss_clearSettings(void);
void ss_loadSettings(void);
void ss_saveSettings(void);
void ss_printSettings(void);

void ss_printHelp(void);

#endif