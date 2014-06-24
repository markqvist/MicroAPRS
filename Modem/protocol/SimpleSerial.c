#define ENABLE_HELP false

#include <stdlib.h>
#include <string.h>
#include <avr/eeprom.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "protocol/SimpleSerial.h"

bool PRINT_SRC = true;
bool PRINT_DST = true;
bool PRINT_PATH = true;
bool PRINT_DATA = true;
bool PRINT_INFO = true;
bool VERBOSE = true;
bool SILENT = false;
bool SS_INIT = false;
bool SS_DEFAULT_CONF = false;

AX25Call src;
AX25Call dst;
AX25Call path1;
AX25Call path2;

char CALL[6] = DEFAULT_CALLSIGN;
int CALL_SSID = 0;
char DST[6] = DEFAULT_DESTINATION_CALL;
int DST_SSID = 0;
char PATH1[6] = "WIDE1";
int PATH1_SSID = 1;
char PATH2[6] = "WIDE2";
int PATH2_SSID = 2;

AX25Call path[4];
AX25Ctx *ax25ctx;

#define NV_MAGIC_BYTE 0x69
uint8_t EEMEM nvMagicByte;
uint8_t EEMEM nvCALL[6];
uint8_t EEMEM nvDST[6];
uint8_t EEMEM nvPATH1[6];
uint8_t EEMEM nvPATH2[6];
uint8_t EEMEM nvCALL_SSID;
uint8_t EEMEM nvDST_SSID;
uint8_t EEMEM nvPATH1_SSID;
uint8_t EEMEM nvPATH2_SSID;
bool EEMEM nvPRINT_SRC;
bool EEMEM nvPRINT_DST;
bool EEMEM nvPRINT_PATH;
bool EEMEM nvPRINT_DATA;
bool EEMEM nvPRINT_INFO;
bool EEMEM nvVERBOSE;
bool EEMEM nvSILENT;
uint8_t EEMEM nvPOWER;
uint8_t EEMEM nvHEIGHT;
uint8_t EEMEM nvGAIN;
uint8_t EEMEM nvDIRECTIVITY;
uint8_t EEMEM nvSYMBOL_TABLE;
uint8_t EEMEM nvSYMBOL;
uint8_t EEMEM nvAUTOACK;

// Location packet assembly fields
char latitude[8];
char longtitude[9];
char symbolTable = '/';
char symbol = 'n';

uint8_t power = 10;
uint8_t height = 10;
uint8_t gain = 10;
uint8_t directivity = 10;
/////////////////////////

// Message packet assembly fields
char message_recip[6];
int message_recip_ssid = -1;

int message_seq = 0;
char lastMessage[67];
size_t lastMessageLen;
bool message_autoAck = false;
/////////////////////////

void ss_init(AX25Ctx *ax25) {
    ax25ctx = ax25;
    ss_loadSettings();
    SS_INIT = true;
    if (VERBOSE) {
        _delay_ms(300);
        kprintf("---------------\n");
        kprintf("MicroAPRS v0.2a\n");
        kprintf("unsigned.io/microaprs\n");
        if (SS_DEFAULT_CONF) kprintf("Default configuration loaded!\n");
        kprintf("Modem ready\n");
        kprintf("---------------\n");
    }
}

void ss_clearSettings(void) {
    eeprom_update_byte((void*)&nvMagicByte, 0xFF);
    if (VERBOSE) kprintf("Configuration cleared. Restart to load defaults.\n");
    if (!VERBOSE && !SILENT) kprintf("1\n");
}

void ss_loadSettings(void) {
    uint8_t verification = eeprom_read_byte((void*)&nvMagicByte);
    if (verification == NV_MAGIC_BYTE) {
        eeprom_read_block((void*)CALL, (void*)nvCALL, 6);
        eeprom_read_block((void*)DST, (void*)nvDST, 6);
        eeprom_read_block((void*)PATH1, (void*)nvPATH1, 6);
        eeprom_read_block((void*)PATH2, (void*)nvPATH2, 6);

        CALL_SSID = eeprom_read_byte((void*)&nvCALL_SSID);
        DST_SSID = eeprom_read_byte((void*)&nvDST_SSID);
        PATH1_SSID = eeprom_read_byte((void*)&nvPATH1_SSID);
        PATH2_SSID = eeprom_read_byte((void*)&nvPATH2_SSID);

        PRINT_SRC = eeprom_read_byte((void*)&nvPRINT_SRC);
        PRINT_DST = eeprom_read_byte((void*)&nvPRINT_DST);
        PRINT_PATH = eeprom_read_byte((void*)&nvPRINT_PATH);
        PRINT_DATA = eeprom_read_byte((void*)&nvPRINT_DATA);
        PRINT_INFO = eeprom_read_byte((void*)&nvPRINT_INFO);
        VERBOSE = eeprom_read_byte((void*)&nvVERBOSE);
        SILENT = eeprom_read_byte((void*)&nvSILENT);

        power = eeprom_read_byte((void*)&nvPOWER);
        height = eeprom_read_byte((void*)&nvHEIGHT);
        gain = eeprom_read_byte((void*)&nvGAIN);
        directivity = eeprom_read_byte((void*)&nvDIRECTIVITY);
        symbolTable = eeprom_read_byte((void*)&nvSYMBOL_TABLE);
        symbol = eeprom_read_byte((void*)&nvSYMBOL);
        message_autoAck = eeprom_read_byte((void*)&nvAUTOACK);

        if (VERBOSE && SS_INIT) kprintf("Configuration loaded\n");
    } else {
        if (SS_INIT && !SILENT && VERBOSE) kprintf("Error: No stored configuration to load!\n");
        if (SS_INIT && !SILENT && !VERBOSE) kprintf("0\n");
        SS_DEFAULT_CONF = true;
    }
}

void ss_saveSettings(void) {
    eeprom_update_block((void*)CALL, (void*)nvCALL, 6);
    eeprom_update_block((void*)DST, (void*)nvDST, 6);
    eeprom_update_block((void*)PATH1, (void*)nvPATH1, 6);
    eeprom_update_block((void*)PATH2, (void*)nvPATH2, 6);

    eeprom_update_byte((void*)&nvCALL_SSID, CALL_SSID);
    eeprom_update_byte((void*)&nvDST_SSID, DST_SSID);
    eeprom_update_byte((void*)&nvPATH1_SSID, PATH1_SSID);
    eeprom_update_byte((void*)&nvPATH2_SSID, PATH2_SSID);

    eeprom_update_byte((void*)&nvPRINT_SRC, PRINT_SRC);
    eeprom_update_byte((void*)&nvPRINT_DST, PRINT_DST);
    eeprom_update_byte((void*)&nvPRINT_PATH, PRINT_PATH);
    eeprom_update_byte((void*)&nvPRINT_DATA, PRINT_DATA);
    eeprom_update_byte((void*)&nvPRINT_INFO, PRINT_INFO);
    eeprom_update_byte((void*)&nvVERBOSE, VERBOSE);
    eeprom_update_byte((void*)&nvSILENT, SILENT);

    eeprom_update_byte((void*)&nvPOWER, power);
    eeprom_update_byte((void*)&nvHEIGHT, height);
    eeprom_update_byte((void*)&nvGAIN, gain);
    eeprom_update_byte((void*)&nvDIRECTIVITY, directivity);
    eeprom_update_byte((void*)&nvSYMBOL_TABLE, symbolTable);
    eeprom_update_byte((void*)&nvSYMBOL, symbol);
    eeprom_update_byte((void*)&nvAUTOACK, message_autoAck);

    eeprom_update_byte((void*)&nvMagicByte, NV_MAGIC_BYTE);

    if (VERBOSE) kprintf("Configuration saved\n");
    if (!VERBOSE && !SILENT) kprintf("1\n");
}

void ss_messageCallback(struct AX25Msg *msg, Serial *ser) {
    if (PRINT_SRC) {
        if (PRINT_INFO) kfile_print(&ser->fd, "SRC: ");
        kfile_printf(&ser->fd, "[%.6s-%d] ", msg->src.call, msg->src.ssid);
    }
    if (PRINT_DST) {
        if (PRINT_INFO) kfile_printf(&ser->fd, "DST: ");
        kfile_printf(&ser->fd, "[%.6s-%d] ", msg->dst.call, msg->dst.ssid);
    }

    if (PRINT_PATH) {
        if (PRINT_INFO) kfile_print(&ser->fd, "PATH: ");
        for (int i = 0; i < msg->rpt_cnt; i++)
            kfile_printf(&ser->fd, "[%.6s-%d] ", msg->rpt_lst[i].call, msg->rpt_lst[i].ssid);
    }
    
    if (PRINT_DATA) {
        if (PRINT_INFO) kfile_print(&ser->fd, "DATA: ");
        kfile_printf(&ser->fd, "%.*s", msg->len, msg->info);
    }
    kfile_print(&ser->fd, "\r\n");

    if (message_autoAck && msg->len > 11) {
        char mseq[6];
        bool shouldAck = true;
        int msl = 0;
        int loc = msg->len - 1;
        size_t i = 0;

        while (i<7 && i < msg->len) {
            if (msg->info[loc-i] == '{') {
                size_t p;
                for (p = 0; p <= i; p++) {
                    mseq[p] = msg->info[loc-i+p];
                    msl = i;
                }
            }
            i++;
        }

        if (msl != 0) {
            int pos = 1;
            int ssidPos = 0;
            while (pos < 7) {
                if (msg->info[pos] != CALL[pos-1]) {
                    shouldAck = false;
                    pos = 7;
                }
                pos++;
            }
            while (pos < 10) {
                if (msg->info[pos] == '-') ssidPos = pos;
                pos++;
            }
            if (ssidPos != 0) {
                if (msg->info[ssidPos+2] == ' ') {
                    if (msg->info[ssidPos+1]-48 != CALL_SSID) {
                        shouldAck = false;
                    }
                } else {
                    int assid = 10+(msg->info[ssidPos+2]-48);
                    if (assid != CALL_SSID) {
                        shouldAck = false;
                    }
                }
            }

            if (msl != 0 && shouldAck) {
                int ii = 0;
                char *ack = malloc(14+msl);

                for (ii = 0; ii < 9; ii++) {
                    ack[1+ii] = ' ';
                }
                int calllen = 0;
                for (ii = 0; ii < 6; ii++) {
                    if (msg->src.call[ii] != 0) {
                        ack[1+ii] = msg->src.call[ii];
                        calllen++;
                    }
                }

                if (msg->src.ssid != 0) {
                    ack[1+calllen] = '-';
                    if (msg->src.ssid < 10) {
                        ack[2+calllen] = msg->src.ssid+48;
                    } else {
                        ack[2+calllen] = 49;
                        ack[3+calllen] = msg->src.ssid-10+48;
                    }
                }

                ack[0] = ':';
                ack[10] = ':';
                ack[11] = 'a';
                ack[12] = 'c';
                ack[13] = 'k';
                
                for (ii = 0; ii < msl; ii++) {
                    ack[14+ii] = mseq[ii+1];
                }

                _delay_ms(1750);
                ss_sendPkt(ack, 14+msl, ax25ctx);

                free(ack);
            }
        }
    }
}

void ss_serialCallback(void *_buffer, size_t length, Serial *ser, AX25Ctx *ctx) {
    uint8_t *buffer = (uint8_t *)_buffer;
    if (length > 0) {
        // ! as first char to send packet
        if (buffer[0] == '!' && length > 1) {
            buffer++; length--;
            ss_sendPkt(buffer, length, ctx);
            if (VERBOSE) kprintf("Packet sent\n");
            if (!VERBOSE && !SILENT) kprintf("1\n");
        } else if (buffer[0] == '@') {
            buffer++; length--;
            ss_sendLoc(buffer, length, ctx);
            if (VERBOSE) kprintf("Location update sent\n");
            if (!VERBOSE && !SILENT) kprintf("1\n");
        } else if (buffer[0] == '#') {
            buffer++; length--;
            ss_sendMsg(buffer, length, ctx);
            if (VERBOSE) kprintf("Message sent\n");
            if (!VERBOSE && !SILENT) kprintf("1\n");
        }
        #if ENABLE_HELP
        else if (buffer[0] == 'h') {
            ss_printHelp();
        }
        #endif
        else if (buffer[0] == 'H') {
            ss_printSettings();
        } else if (buffer[0] == 'S') {
            ss_saveSettings();
        } else if (buffer[0] == 'C') {
            ss_clearSettings();
        } else if (buffer[0] == 'L') {
            ss_loadSettings();
        } else if (buffer[0] == 'c' && length > 3) {
            buffer++; length--;
            int count = 0;
            while (length-- && count < 6) {
                char c = buffer[count];
                if (c != 0 && c != 10 && c != 13) {
                    CALL[count] = c;
                } else {
                    CALL[count] = 0x00;
                }
                count++;
            }
            while (count < 6) {
                CALL[count] = 0x00;
                count++;
            }
            if (VERBOSE) kprintf("Callsign: %.6s-%d\n", CALL, CALL_SSID);
            if (!VERBOSE && !SILENT) kprintf("1\n");

        } else if (buffer[0] == 'd' && length > 3) {
            buffer++; length--;
            int count = 0;
            while (length-- && count < 6) {
                char c = buffer[count];
                if (c != 0 && c != 10 && c != 13) {
                    DST[count] = c;
                } else {
                    DST[count] = 0;
                }
                count++;
            }
            while (count < 6) {
                DST[count] = 0x00;
                count++;
            }
            if (VERBOSE) kprintf("Destination: %.6s-%d\n", DST, DST_SSID);
            if (!VERBOSE && !SILENT) kprintf("1\n");


        } else if (buffer[0] == '1' && length > 1) {
            buffer++; length--;
            int count = 0;
            while (length-- && count < 6) {
                char c = buffer[count];
                if (c != 0 && c != 10 && c != 13) {
                    PATH1[count] = c;
                } else {
                    PATH1[count] = 0;
                }
                count++;
            }
            while (count < 6) {
                PATH1[count] = 0x00;
                count++;
            }
            if (VERBOSE) kprintf("Path1: %.6s-%d\n", PATH1, PATH1_SSID);
            if (!VERBOSE && !SILENT) kprintf("1\n");


        } else if (buffer[0] == '2' && length > 1) {
            buffer++; length--;
            int count = 0;
            while (length-- && count < 6) {
                char c = buffer[count];
                if (c != 0 && c != 10 && c != 13) {
                    PATH2[count] = c;
                } else {
                    PATH2[count] = 0;
                }
                count++;
            }
            while (count < 6) {
                PATH2[count] = 0x00;
                count++;
            }
            if (VERBOSE) kprintf("Path2: %.6s-%d\n", PATH2, PATH2_SSID);
            if (!VERBOSE && !SILENT) kprintf("1\n");


        } else if (buffer[0] == 's' && length > 2) {
            buffer++; length--;
            if (buffer[0] == 'c') {
                if (length > 2 && buffer[2] > 48 && buffer[2] < 58) {
                    CALL_SSID = 10+buffer[2]-48;
                } else {
                    CALL_SSID = buffer[1]-48;
                }
                if (VERBOSE) kprintf("Callsign: %.6s-%d\n", CALL, CALL_SSID);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            }
            if (buffer[0] == 'd') {
                if (length > 2 && buffer[2] > 48 && buffer[2] < 58) {
                    DST_SSID = 10+buffer[2]-48;
                } else {
                    DST_SSID = buffer[1]-48;
                }
                if (VERBOSE) kprintf("Destination: %.6s-%d\n", DST, DST_SSID);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            }
            if (buffer[0] == '1' && buffer[2] > 48 && buffer[2] < 58) {
                if (length > 2) {
                    PATH1_SSID = 10+buffer[2]-48;
                } else {
                    PATH1_SSID = buffer[1]-48;
                }
                if (VERBOSE) kprintf("Path1: %.6s-%d\n", PATH1, PATH1_SSID);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            }
            if (buffer[0] == '2' && buffer[2] > 48 && buffer[2] < 58) {
                if (length > 2) {
                    PATH2_SSID = 10+buffer[2]-48;
                } else {
                    PATH2_SSID = buffer[1]-48;
                }
                if (VERBOSE) kprintf("Path2: %.6s-%d\n", PATH2, PATH2_SSID);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            }
            
        } else if (buffer[0] == 'p' && length > 2) {
            buffer++; length--;
            if (buffer[0] == 's') {
                if (buffer[1] == 49) {
                    PRINT_SRC = true;
                    if (VERBOSE) kprintf("Print SRC enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    PRINT_SRC = false;
                    if (VERBOSE) kprintf("Print SRC disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }
            if (buffer[0] == 'd') {
                if (buffer[1] == 49) {
                    PRINT_DST = true;
                    if (VERBOSE) kprintf("Print DST enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    PRINT_DST = false;
                    if (VERBOSE) kprintf("Print DST disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }
            if (buffer[0] == 'p') {
                if (buffer[1] == 49) {
                    PRINT_PATH = true;
                    if (VERBOSE) kprintf("Print PATH enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    PRINT_PATH = false;
                    if (VERBOSE) kprintf("Print PATH disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }
            if (buffer[0] == 'm') {
                if (buffer[1] == 49) {
                    PRINT_DATA = true;
                    if (VERBOSE) kprintf("Print DATA enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    PRINT_DATA = false;
                    if (VERBOSE) kprintf("Print DATA disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }
            if (buffer[0] == 'i') {
                if (buffer[1] == 49) {
                    PRINT_INFO = true;
                    if (VERBOSE) kprintf("Print INFO enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    PRINT_INFO = false;
                    if (VERBOSE) kprintf("Print INFO disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }
        } else if (buffer[0] == 'v') {
            if (buffer[1] == 49) {
                VERBOSE = true;
                kfile_printf(&ser->fd, "Verbose mode enabled\n");
            } else {
                VERBOSE = false;
                kfile_printf(&ser->fd, "Verbose mode disabled\n");
            }
        } else if (buffer[0] == 'V') {
            if (buffer[1] == 49) {
                SILENT = true;
                VERBOSE = false;
                kfile_printf(&ser->fd, "Silent mode enabled\n");
            } else {
                SILENT = false;
                kfile_printf(&ser->fd, "Silent mode disabled\n");
            }
        } else if (buffer[0] == 'l' && length > 2) {
            buffer++; length--;
            if (buffer[0] == 'l' && buffer[1] == 'a' && length >= 10) {
                buffer += 2;
                memcpy(latitude, (void *)buffer, 8);
                if (VERBOSE) kprintf("Latitude set to %.8s\n", latitude);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            } else if (buffer[0] == 'l' && buffer[1] == 'o' && length >= 11) {
                buffer += 2;
                memcpy(longtitude, (void *)buffer, 9);
                if (VERBOSE) kprintf("Longtitude set to %.9s\n", longtitude);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            } else if (buffer[0] == 'p' && length >= 2 && buffer[1] >= 48 && buffer[1] <= 57) {
                power = buffer[1] - 48;
                if (VERBOSE) kprintf("Power set to %dw\n", power*power);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            } else if (buffer[0] == 'h' && length >= 2 && buffer[1] >= 48 && buffer[1] <= 57) {
                height = buffer[1] - 48;
                if (VERBOSE) kprintf("Antenna height set to %ldm AAT\n", (long)(BV(height)*1000L)/328L);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            } else if (buffer[0] == 'g' && length >= 2 && buffer[1] >= 48 && buffer[1] <= 57) {
                gain = buffer[1] - 48;
                if (VERBOSE) kprintf("Gain set to %ddB\n", gain);
                if (!VERBOSE && !SILENT) kprintf("1\n");
            } else if (buffer[0] == 'd' && length >= 2 && buffer[1] >= 48 && buffer[1] <= 57) {
                directivity = buffer[1] - 48;
                if (directivity == 9) directivity = 8;
                if (!VERBOSE && !SILENT) kprintf("1\n");
                if (VERBOSE) {
                    if (directivity == 0) kprintf("Directivity set to omni\n");
                    if (directivity != 0) kprintf("Directivity set to %ddeg\n", directivity*45);
                }
            } else if (buffer[0] == 's' && length >= 2) {
                symbol = buffer[1];
                if (VERBOSE) kprintf("Symbol set to %c\n", symbol);
            } else if (buffer[0] == 't' && length >= 2) {
                if (buffer[1] == 'a') {
                    symbolTable = '\\';
                    if (VERBOSE) kprintf("Selected alternate symbol table\n");
                } else {
                    symbolTable = '/';
                    if (VERBOSE) kprintf("Selected standard symbol table\n");
                }
                if (!VERBOSE && !SILENT) kprintf("1\n");
            }


        } else if (buffer[0] == 'm' && length > 1) {
            buffer++; length--;
            if (buffer[0] == 'c' && length > 1) {
                buffer++; length--;
                int count = 0;
                while (length-- && count < 6) {
                    char c = buffer[count];
                    if (c != 0 && c != 10 && c != 13) {
                        message_recip[count] = c;
                    } else {
                        message_recip[count] = 0x00;
                    }
                    count++;
                }
                while (count < 6) {
                    message_recip[count] = 0x00;
                    count++;
                }
                if (VERBOSE) {
                    kprintf("Message recipient: %.6s", message_recip);
                    if (message_recip_ssid != -1) { 
                        kprintf("-%d\n", message_recip_ssid);
                    } else {
                        kprintf("\n");
                    }
                }
                if (!VERBOSE && !SILENT) kprintf("1\n");
            } else if (buffer[0] == 's' && length > 1) {
                if (length > 2) {
                    message_recip_ssid = 10+buffer[2]-48;
                } else {
                    message_recip_ssid = buffer[1]-48;
                }
                if (message_recip_ssid < 0 || message_recip_ssid > 15) message_recip_ssid = -1;
                if (VERBOSE) {
                    kprintf("Message recipient: %.6s", message_recip);
                    if (message_recip_ssid != -1) { 
                        kprintf("-%d\n", message_recip_ssid);
                    } else {
                        kprintf("\n");
                    }
                }
                if (!VERBOSE && !SILENT) kprintf("1\n");
            } else if (buffer[0] == 'r') {
                ss_msgRetry(ctx);
                if (VERBOSE) kprintf("Retried last message\n");
                if (!VERBOSE && !SILENT) kprintf("1\n");
            } else if (buffer[0] == 'a') {
                if (buffer[1] == 49) {
                    message_autoAck = true;
                    if (VERBOSE) kprintf("Message auto-ack enabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                } else {
                    message_autoAck = false;
                    if (VERBOSE) kprintf("Message auto-ack disabled\n");
                    if (!VERBOSE && !SILENT) kprintf("1\n");
                }
            }

        } else {
            if (VERBOSE) kprintf("Error: Invalid command\n");
            if (!VERBOSE && !SILENT) kprintf("0\n");
        }

    }

}

void ss_sendPkt(void *_buffer, size_t length, AX25Ctx *ax25) {

    uint8_t *buffer = (uint8_t *)_buffer;

    memcpy(dst.call, DST, 6);
    dst.ssid = DST_SSID;

    memcpy(src.call, CALL, 6);
    src.ssid = CALL_SSID;

    memcpy(path1.call, PATH1, 6);
    path1.ssid = PATH1_SSID;

    memcpy(path2.call, PATH2, 6);
    path2.ssid = PATH2_SSID;

    path[0] = dst;
    path[1] = src;
    path[2] = path1;
    path[3] = path2;

    ax25_sendVia(ax25, path, countof(path), buffer, length);
}

void ss_sendLoc(void *_buffer, size_t length, AX25Ctx *ax25) {
    size_t payloadLength = 20+length;
    bool usePHG = false;
    if (power < 10 && height < 10 && gain < 10 && directivity < 9) {
        usePHG = true;
        payloadLength += 7;
    }
    uint8_t *packet = malloc(payloadLength);
    uint8_t *ptr = packet;
    packet[0] = '=';
    packet[9] = symbolTable;
    packet[19] = symbol;
    ptr++;
    memcpy(ptr, latitude, 8);
    ptr += 9;
    memcpy(ptr, longtitude, 9);
    ptr += 10;
    if (usePHG) {
        packet[20] = 'P';
        packet[21] = 'H';
        packet[22] = 'G';
        packet[23] = power+48;
        packet[24] = height+48;
        packet[25] = gain+48;
        packet[26] = directivity+48;
        ptr+=7;
    }
    if (length > 0) {
        uint8_t *buffer = (uint8_t *)_buffer;
        memcpy(ptr, buffer, length);
    }

    //kprintf("Assembled packet:\n%.*s\n", payloadLength, packet);
    ss_sendPkt(packet, payloadLength, ax25);
    free(packet);
}

void ss_sendMsg(void *_buffer, size_t length, AX25Ctx *ax25) {
    if (length > 67) length = 67;
    size_t payloadLength = 11+length+4;

    uint8_t *packet = malloc(payloadLength);
    uint8_t *ptr = packet;
    packet[0] = ':';
    int callSize = 6;
    int count = 0;
    while (callSize--) {
        if (message_recip[count] != 0) {
            packet[1+count] = message_recip[count];
            count++;
        }
    }
    if (message_recip_ssid != -1) {
        packet[1+count] = '-'; count++;
        if (message_recip_ssid < 10) {
            packet[1+count] = message_recip_ssid+48; count++;
        } else {
            packet[1+count] = 49; count++;
            packet[1+count] = message_recip_ssid-10+48; count++;
        }
    }
    while (count < 9) {
        packet[1+count] = ' '; count++;
    }
    packet[1+count] = ':';
    ptr += 11;
    if (length > 0) {
        uint8_t *buffer = (uint8_t *)_buffer;
        memcpy(ptr, buffer, length);
        memcpy(lastMessage, buffer, length);
        lastMessageLen = length;
    }

    message_seq++;
    if (message_seq > 999) message_seq = 0;

    packet[11+length] = '{';
    int n = message_seq % 10;
    int d = ((message_seq % 100) - n)/10;
    int h = (message_seq - d - n) / 100;

    packet[12+length] = h+48;
    packet[13+length] = d+48;
    packet[14+length] = n+48;
    
    //kprintf("Assembled packet:\n%.*s\n", payloadLength, packet);
    ss_sendPkt(packet, payloadLength, ax25);

    free(packet);
}

void ss_msgRetry(AX25Ctx *ax25) {
    message_seq--;
    ss_sendMsg(lastMessage, lastMessageLen, ax25);
}

void ss_printSettings(void) {
    kprintf("Configuration:\n");
    kprintf("Callsign: %.6s-%d\n", CALL, CALL_SSID);
    kprintf("Destination: %.6s-%d\n", DST, DST_SSID);
    kprintf("Path1: %.6s-%d\n", PATH1, PATH1_SSID);
    kprintf("Path2: %.6s-%d\n", PATH2, PATH2_SSID);
    if (message_autoAck) {
        kprintf("Auto-ack messages: On\n");
    } else {
        kprintf("Auto-ack messages: Off\n");
    }
    if (power != 10) kprintf("Power: %d\n", power);
    if (height != 10) kprintf("Height: %d\n", height);
    if (gain != 10) kprintf("Gain: %d\n", gain);
    if (directivity != 10) kprintf("Directivity: %d\n", directivity);
    if (symbolTable == '\\') kprintf("Symbol table: alternate\n");
    if (symbolTable == '/') kprintf("Symbol table: standard\n");
    kprintf("Symbol: %c\n", symbol);
}

#if ENABLE_HELP
    void ss_printHelp(void) {
            kprintf("----------------------------------\n");
            kprintf("Serial commands:\n");
            kprintf("!<data>   Send raw packet\n");
            kprintf("@<cmt>    Send location update (cmt = optional comment)\n");
            kprintf("#<msg>    Send APRS message\n\n");

            kprintf("c<call>   Set your callsign\n");
            kprintf("d<call>   Set destination callsign\n");
            kprintf("1<call>   Set PATH1 callsign\n");
            kprintf("2<call>   Set PATH2 callsign\n\n");

            kprintf("sc<ssid>  Set your SSID\n");
            kprintf("sd<ssid>  Set destination SSID\n");
            kprintf("s1<ssid>  Set PATH1 SSID\n");
            kprintf("s2<ssid>  Set PATH2 SSID\n\n");

            kprintf("lla<LAT>  Set latitude (NMEA-format, eg 4903.50N)\n");
            kprintf("llo<LON>  Set latitude (NMEA-format, eg 07201.75W)\n");
            kprintf("lp<0-9>   Set TX power info\n");
            kprintf("lh<0-9>   Set antenna height info\n");
            kprintf("lg<0-9>   Set antenna gain info\n");
            kprintf("ld<0-9>   Set antenna directivity info\n");
            kprintf("ls<sym>   Select symbol\n");
            kprintf("lt<s/a>   Select symbol table (standard/alternate)\n\n");

            kprintf("mc<call>  Set message recipient callsign\n");
            kprintf("ms<ssid>  Set message recipient SSID\n");
            kprintf("mr<ssid>  Retry last message\n");
            kprintf("ma<1/0>   Automatic message ACK on/off\n\n");

            kprintf("ps<1/0>   Print SRC on/off\n");
            kprintf("pd<1/0>   Print DST on/off\n");
            kprintf("pp<1/0>   Print PATH on/off\n");
            kprintf("pm<1/0>   Print DATA on/off\n");
            kprintf("pi<1/0>   Print INFO on/off\n\n");
            kprintf("v<1/0>    Verbose mode on/off\n");
            kprintf("V<1/0>    Silent mode on/off\n\n");

            kprintf("S         Save configuration\n");
            kprintf("L         Load configuration\n");
            kprintf("C         Clear configuration\n");
            kprintf("H         Print configuration\n");
            kprintf("----------------------------------\n");
    }
#endif