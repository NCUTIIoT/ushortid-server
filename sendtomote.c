#include <stdio.h>
#include <string.h> //for memset
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "sendtomote.h"

static int sendtomote_send(const char *moteaddr, const char *msg, unsigned int dstport, size_t msglen)
{
    char dstport_s[8];
    struct addrinfo hints, *result;
    size_t sendlen;
    int s, sfd; //socket id

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    //V6,V4
    hints.ai_socktype = SOCK_DGRAM; //UDP
    hints.ai_flags = 0;
    sprintf(dstport_s, "%u", dstport);
    s = getaddrinfo(moteaddr, dstport_s, &hints, &result);
    if (s != 0)
        return -1;

    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd < 0)
        return -1;

    sendlen = sendto(sfd, msg, msglen, 0, result->ai_addr, result->ai_addrlen);
    close(sfd);

    return (sendlen == msglen) ? 0 : 1;
}

void *sendtomote_send_entry(void *ptr)
{
    data_to_mote_t *d = (data_to_mote_t *)ptr;
    usleep(d->delayInMS);
    sendtomote_send(d->moteaddr, d->msg, d->dstport, d->msglen);
    free((char *)d->moteaddr);
    free((char *)d->msg);
    free(d);
    return 0;
}
