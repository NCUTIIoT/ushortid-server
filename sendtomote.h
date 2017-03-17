#ifndef _SEWND_TO_MOTE_H
#define _SEWND_TO_MOTE_H

#include <sys/socket.h>

typedef struct
{
    const char *moteaddr;
    const char *msg;
    unsigned int dstport;
    int delayInMS;
    size_t msglen;
} data_to_mote_t;

void *sendtomote_send_entry(void *ptr);

#endif
