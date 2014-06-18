#include <string.h>
#include "protocol/SimpleSerial.h"

bool PRINT_SRC = true;
bool PRINT_DST = true;
bool PRINT_PATH = true;
bool PRINT_DATA = true;
bool PRINT_INFO = true;
bool VERBOSE = false;


AX25Call src;
AX25Call dst;
AX25Call path1;
AX25Call path2;

char CALL[6] = "NOCALL";
int CALL_SSID = 0;
char DST[6] = "APZMDM";
int DST_SSID = 0;
char PATH1[6] = "WIDE1";
int PATH1_SSID = 1;
char PATH2[6] = "WIDE2";
int PATH2_SSID = 2;

AX25Call path[4];

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
}

void ss_serialCallback(void *_buffer, size_t length, Serial *ser, AX25Ctx *ctx) {
    if (VERBOSE) {
        kfile_printf(&ser->fd, "Serial input");
    }

    uint8_t *buffer = (uint8_t *)_buffer;
    if (length > 0) {
        // ! as first char to send packet
        if (buffer[0] == '!' && length > 1) {
            buffer++; length--;
            ss_sendMsg(buffer, length, ctx);
        } else if (buffer[0] == 'c' && length > 3) {
            buffer++; length--;
            int count = 0;
            while (length-- && count < 6) {
                char c = buffer[count];
                if (c != 0 && c != 10 && c != 13) {
                    CALL[count] = c;
                } else {
                    CALL[count] = 0;
                }
                count++;
            }

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


        } else if (buffer[0] == 's' && length > 2) {
            buffer++; length--;
            if (buffer[0] == 'c') CALL_SSID = buffer[1]-48;
            if (buffer[0] == 'd') DST_SSID = buffer[1]-48;
            if (buffer[0] == '1') PATH1_SSID = buffer[1]-48;
            if (buffer[0] == '2') PATH2_SSID = buffer[1]-48;
            
        } else if (buffer[0] == 'p' && length > 2) {
            buffer++; length--;
            if (buffer[0] == 's') {
                if (buffer[1] == 49) { PRINT_SRC = true; } else { PRINT_SRC = false; }
            }
            if (buffer[0] == 'd') {
                if (buffer[1] == 49) { PRINT_DST = true; } else { PRINT_DST = false; }
            }
            if (buffer[0] == 'p') {
                if (buffer[1] == 49) { PRINT_PATH = true; } else { PRINT_PATH = false; }
            }
            if (buffer[0] == 'm') {
                if (buffer[1] == 49) { PRINT_DATA = true; } else { PRINT_DATA = false; }
            }
            if (buffer[0] == 'i') {
                if (buffer[1] == 49) { PRINT_INFO = true; } else { PRINT_INFO = false; }
            }

        }

    }

}

void ss_sendMsg(void *_buffer, size_t length, AX25Ctx *ax25) {

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

void ss_printSrc(bool val) {
    PRINT_SRC = val;
}

void ss_printDst(bool val) {
    PRINT_DST = val;
}

void ss_printPath(bool val) {
    PRINT_PATH = val;
}

void ss_printData(bool val) {
    PRINT_DATA = val;
}

void ss_printInfo(bool val) {
    PRINT_INFO = val;
}
