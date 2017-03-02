#include <stdio.h>
#include <string.h> //for memset
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "avl.h"

typedef struct
{
    unsigned char addr64b[8];
    unsigned short id;
} MOTE_SHORT_ADDR;

static int cmpId(void *a, void *b)
{
    MOTE_SHORT_ADDR *c, *d;

    c = (MOTE_SHORT_ADDR *)a;
    d = (MOTE_SHORT_ADDR *)b;

    if (c->id > d->id)
        return 1;
    else if (c->id < d->id)
        return -1;
    else
        return 0;
}

static int cmpAddr(void *a, void *b)
{
    MOTE_SHORT_ADDR *c, *d;

    c = (MOTE_SHORT_ADDR *)a;
    d = (MOTE_SHORT_ADDR *)b;

    return memcmp(c->addr64b, d->addr64b, sizeof(c->addr64b));
}

static void _releaser(void *ptr)
{
    free(ptr);
}

static AVL_TREE *treeById, *treeByAddr;
static int ushortid_server(unsigned int port)
{
    char buf[256];
    char ipstr[INET6_ADDRSTRLEN], moteaddr_str[32];
    MOTE_SHORT_ADDR received, sendbuf, *ptr;
    struct sockaddr_in6 from;
    NODE *cur;
    ssize_t recvlen;
    socklen_t fromlen;
    unsigned short nextIndex, id;

    int sfd, sendsfd; //socket id
    sfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sfd < 0)
    {
        printf("socket creat erro\n");
        return -1;
    }

    treeById = AVL_Create(cmpId, _releaser);
    treeByAddr = AVL_Create(cmpAddr, 0);
    memset(&from, 0, sizeof(struct sockaddr_in6));
    from.sin6_family = AF_INET6;
    from.sin6_port = htons(port);
    from.sin6_addr = in6addr_any;
    bind(sfd, (const struct sockaddr *)&(from), sizeof(from));
    fromlen = sizeof(from);
    while ((recvlen = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen)) != -1)
    {
        inet_ntop(from.sin6_family, &(from.sin6_addr), ipstr, sizeof(ipstr));
        //printf("recvfrom %s:%u\n", ipstr, port);
        if (recvlen != sizeof(received))
        {
            printf("[ERROR] Protocol error(datagram size=%i) from %s, port=%hu.\n", (int)recvlen, ipstr, ntohs(from.sin6_port));
            continue;
        }

        memcpy(&received, buf, sizeof(received));
        sprintf(moteaddr_str, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", (unsigned char)buf[0], (unsigned char)buf[1], (unsigned char)buf[2], (unsigned char)buf[3], (unsigned char)buf[4], (unsigned char)buf[5], (unsigned char)buf[6], (unsigned char)buf[7]);
        printf("[INFO] recv from %s, (%s => %u)\n", ipstr, moteaddr_str, received.id);
        if ((ptr = AVL_Retrieve(treeByAddr, &received)))
        {
            printf("[INFO] found, (%s => %u)\n", moteaddr_str, ptr->id);
        }
        else
        {
            cur = treeById->root;
            if (cur)
            {
                while (cur->right)
                    cur = cur->right;
                nextIndex = ((MOTE_SHORT_ADDR *)(cur->dataPtr))->id + 1;
            }
            else
            {
                nextIndex = 1;
            }
            printf("[INFO] next index = %u\n", nextIndex);

            received.id = nextIndex;
            ptr = malloc(sizeof(*ptr));
            memcpy(ptr, &received, sizeof(*ptr));
            AVL_Insert(treeById, ptr);
            AVL_Insert(treeByAddr, ptr);
            printf("[INFO] created, (%s => %u)\n", moteaddr_str, ptr->id);
        }

        id = ptr->id;
        memcpy(&(sendbuf.addr64b), ptr->addr64b, sizeof(sendbuf.addr64b));
        ((unsigned char *)(&(sendbuf.id)))[0] = (id & 0xFF00) >> 8;
        ((unsigned char *)(&(sendbuf.id)))[1] = (id & 0x00FF) >> 0;
        sendsfd = socket(AF_INET6, SOCK_DGRAM, 0);
        if (sendsfd >= 0)
        {
            sendto(sendsfd, &sendbuf, sizeof(sendbuf), 0, (const struct sockaddr *)&from, fromlen);
            printf("[INFO] %hu => %s (from %s)\n", id, moteaddr_str, ipstr);
        }
        close(sendsfd);
    }

    close(sfd);
    AVL_Destroy(treeByAddr);
    AVL_Destroy(treeById);
    return 0;
}

void *ushortid_server_entry(void *port_uint_ptr)
{
    unsigned int port = *(unsigned int *)port_uint_ptr;
    ushortid_server(port);
    return 0;
}

const unsigned char *ushortid_server_getAddrById(unsigned short id)
{
    MOTE_SHORT_ADDR key, *ptr;

    ((unsigned char *)(&(key.id)))[0] = (id & 0xFF00) >> 8;
    ((unsigned char *)(&(key.id)))[1] = (id & 0x00FF) >> 0;

    ptr = AVL_Retrieve(treeByAddr, &key);
    if (ptr)
        return ptr->addr64b;
    else
        return 0;
}