#include <stdio.h>
#include <string.h> //for memset
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "avl.h"
#include "ushortid-server.h"

typedef struct
{
    unsigned char addr64b[8];
    time_t lastUpdate;
} MOTE_REPORT;

static int cmp(void *a, void *b)
{
    MOTE_REPORT *c, *d;

    c = (MOTE_REPORT *)a;
    d = (MOTE_REPORT *)b;

    return memcmp(c->addr64b, d->addr64b, sizeof(c->addr64b));
}

static void _releaser(void *ptr)
{
    free(ptr);
}

extern unsigned int nRoots;
extern char **roots;
static int uhurricane_listener(unsigned int port)
{
    unsigned char buf[256];
    char ipstr[INET6_ADDRSTRLEN];
    struct sockaddr_in6 from;
    AVL_TREE *motes = AVL_Create(cmp, _releaser);
    MOTE_REPORT *n, *r;
    const unsigned char *bufaddr;
    ssize_t recvlen;
    socklen_t fromlen;
    unsigned short sid;

    int sfd; //socket id
    sfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sfd < 0)
    {
        printf("socket creat erro\n");
        return -1;
    }

    memset(&from, 0, sizeof(struct sockaddr_in6));
    from.sin6_family = AF_INET6;
    from.sin6_port = htons(port);
    from.sin6_addr = in6addr_any;
    bind(sfd, (const struct sockaddr *)&(from), sizeof(from));
    fromlen = sizeof(from);
    while ((recvlen = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen)) != -1)
    {
        inet_ntop(from.sin6_family, &(from.sin6_addr), ipstr, sizeof(ipstr));
        sid = (unsigned short)((buf[1] << 8) + (buf[2] << 0));
        bufaddr = ushortid_server_getAddrById(sid);
        if (bufaddr)
        {
            n = malloc(sizeof(*n));
            memcpy(n->addr64b, bufaddr, sizeof(n->addr64b));
            r = AVL_Retrieve(motes, n);
            if (!r)
            {
                n->lastUpdate = time(0);
                AVL_Insert(motes, n);
            }
            else
            {
                free(n);
                r->lastUpdate = time(0);
                n = r;
            }
        }
    }

    close(sfd);
    AVL_Destroy(motes);
    return 0;
}

void *uhurricane_listener_entry(void *port_uint_ptr)
{
    unsigned int port = *(unsigned int *)port_uint_ptr;
    uhurricane_listener(port);
    return 0;
}