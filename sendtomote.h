#ifndef _SEWND_TO_MOTE_H
#define _SEWND_TO_MOTE_H

#include <sys/socket.h>
int sendtomote_send(const char *moteaddr, const char *msg, unsigned int dstport, size_t msglen);

#endif
