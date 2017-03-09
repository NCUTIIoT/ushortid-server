#include <stdio.h>
#include <string.h> //for memset
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "avl.h"
#include "ushortid-server.h"

#define TIMEOUT_MOTE_UPDATE 60

typedef struct
{
    unsigned char addr64b[8];
    time_t lastUpdate;
} MOTE_REPORT;

typedef struct
{
    unsigned char addr1[8];
    unsigned char addr2[8];
    double pdr;
    time_t lastUpdate;
} MOTE_REPORT_NEI;

static int cmp(void *a, void *b)
{
    MOTE_REPORT *c, *d;

    c = (MOTE_REPORT *)a;
    d = (MOTE_REPORT *)b;

    return memcmp(c->addr64b, d->addr64b, sizeof(c->addr64b));
}

static int cmpNei(void *a, void *b)
{
    MOTE_REPORT_NEI *c, *d;
    int r;

    c = (MOTE_REPORT_NEI *)a;
    d = (MOTE_REPORT_NEI *)b;

    if ((r = memcmp(c->addr1, d->addr1, sizeof(c->addr1))) != 0)
        return r;
    else if ((r = memcmp(c->addr2, d->addr2, sizeof(c->addr2))) != 0)
        return r;
    else
        return 0;
}

static void _releaser(void *ptr)
{
    free(ptr);
}

extern unsigned int nRoots;
extern unsigned char **roots;
AVL_TREE *motes, *connections;
static int uhurricane_listener(unsigned int port)
{
    unsigned char buf[256];
    char ipstr[INET6_ADDRSTRLEN];
    struct sockaddr_in6 from;
    double pdr;
    MOTE_REPORT *n, *r, *c;
    MOTE_REPORT_NEI *nei, *neir;
    const unsigned char *bufaddr;
    ssize_t recvlen, i;
    socklen_t fromlen;
    unsigned short sid;

    int sfd; //socket id
    sfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sfd < 0)
    {
        printf("socket creat erro\n");
        return -1;
    }

    motes = AVL_Create(cmp, _releaser);
    connections = AVL_Create(cmpNei, _releaser);

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
            c = n;
            for (i = 7; i < recvlen; i += 6)
            {
                sid = (unsigned short)((buf[i + 0] << 8) + (buf[i + 1] << 0));
                bufaddr = ushortid_server_getAddrById(sid);
                if (bufaddr)
                {
                    n = malloc(sizeof(*n));
                    memcpy(n->addr64b, bufaddr, sizeof(n->addr64b));
                    r = n;
                    if (*((unsigned char *)buf + (i + 4)) > 0)
                        pdr = (double)(*((unsigned char *)buf + (i + 5))) / (double)(*((unsigned char *)buf + (i + 4)));
                    else
                        pdr = 0.0;
                    nei = (MOTE_REPORT_NEI *)malloc(sizeof(*nei));
                    nei->pdr = pdr;
                    if (cmp(c, r) > 0)
                    {
                        // c > r
                        memcpy(nei->addr1, r->addr64b, sizeof(nei->addr1));
                        memcpy(nei->addr2, c->addr64b, sizeof(nei->addr2));
                    }
                    else
                    {
                        // c < r
                        memcpy(nei->addr1, c->addr64b, sizeof(nei->addr1));
                        memcpy(nei->addr2, r->addr64b, sizeof(nei->addr2));
                    }
                    neir = AVL_Retrieve(connections, nei);
                    if (neir)
                    {
                        neir->lastUpdate = time(0);
                        free(nei);
                    }
                    else
                    {
                        AVL_Insert(connections, nei);
                        nei->lastUpdate = time(0);
                    }
                    free(n);
                }
            }
        }
    }

    close(sfd);
    AVL_Destroy(motes);
    AVL_Destroy(connections);
    return 0;
}

void *uhurricane_listener_entry(void *port_uint_ptr)
{
    unsigned int port = *(unsigned int *)port_uint_ptr;
    uhurricane_listener(port);
    return 0;
}

static void _traveserM(void *ptr, void *param)
{
    MOTE_REPORT *m;
    FILE *f;
    time_t n, c;
    unsigned char rootFlag = 0x00;

    m = (MOTE_REPORT *)ptr;
    f = (FILE *)param;

    n = time(0);
    c = m->lastUpdate;

    if (n - c <= TIMEOUT_MOTE_UPDATE)
    {
        fwrite(m->addr64b, 1, 8, f);
        fwrite(&rootFlag, 1, 1, f);
    }
}

static void _traveserC(void *ptr, void *param)
{
    MOTE_REPORT_NEI *m;
    FILE *f;
    time_t n, c;

    m = (MOTE_REPORT_NEI *)ptr;
    f = (FILE *)param;
    n = time(0);
    c = m->lastUpdate;

    if (n - c <= TIMEOUT_MOTE_UPDATE)
    {
        fwrite(m->addr1, 1, 8, f);
        fwrite(m->addr2, 1, 8, f);
        fwrite(&(m->pdr), 1, sizeof(m->pdr), f);
    }
}

int uhurricane_listener_writeToFile(const char *mfilename, const char *cfilename)
{
    unsigned int i;
    FILE *f = fopen(mfilename, "wb");
    unsigned char rootFlag = 0x01;

    if (!f)
        return errno;
    for (i = 0; i < nRoots; i += 1)
    {
        if (fwrite(roots[i], 1, 8, f) != 8)
        {
            fclose(f);
            return errno;
        }
        if (fwrite(&rootFlag, 1, 1, f) != 1)
        {
            fclose(f);
            return errno;
        }
    }
    AVL_Traverse(motes, f, _traveserM);
    if (ferror(f))
    {
        fclose(f);
        return errno;
    }
    fclose(f);

    f = fopen(cfilename, "wb");
    if (!f)
        return errno;
    AVL_Traverse(connections, f, _traveserC);
    if (ferror(f))
    {
        fclose(f);
        return errno;
    }
    fclose(f);

    return 0;
}
