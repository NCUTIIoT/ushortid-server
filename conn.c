#include "conn.h"

#include <stdlib.h>
#include <string.h>

typedef struct
{
    double pdr;
    unsigned int id1;
    unsigned int id2;
} Conn_t;

static int _cmp_conns(void *a, void *b)
{
    Conn_t *c, *d;

    c = (Conn_t *)a;
    d = (Conn_t *)b;

    if (c->id1 > d->id1)
        return 1;
    else if (c->id1 < d->id1)
        return -1;
    else if (c->id2 > d->id2)
        return 1;
    else if (c->id2 < d->id2)
        return -1;
    else
        return 0;
}

static void _releaser(void *a)
{
    free(a);
}

void conn_init(Conns_t *conns)
{
    conns->tree = AVL_Create(_cmp_conns, _releaser);
}

void conn_destroy(Conns_t *conns)
{
    AVL_Destroy(conns->tree);
}

int conn_setPdr(Conns_t *conns, WirelessNodes_t *wnodes, unsigned char *addr1, unsigned char *addr2, double pdr)
{
    Conn_t c, *n;
    WirelessNode_t *ptr;
    unsigned int t;

    ptr = wnode_findByAddr(addr1, wnodes);
    if (!ptr)
        return 3;
    c.id1 = ptr->nid;
    ptr = wnode_findByAddr(addr2, wnodes);
    if (!ptr)
        return 4;
    c.id2 = ptr->nid;

    if (c.id1 == c.id2)
        return 1;
    else if (c.id1 > c.id2)
    {
        t = c.id1;
        c.id1 = c.id2;
        c.id2 = t;
    }
    c.pdr = pdr;
    n = malloc(sizeof(*n));
    memcpy(n, &c, sizeof(*n));
    AVL_Insert(conns->tree, n);
    return 0;
}

double conn_getPdr(Conns_t *conns, unsigned int id1, unsigned int id2)
{
    Conn_t c, *res;
    unsigned int t;

    if (id1 == id2)
        return 1.0;
    else if (id1 > id2)
    {
        t = id1;
        id1 = id2;
        id2 = t;
    }

    c.id1 = id1;
    c.id2 = id2;

    res = (Conn_t *)AVL_Retrieve(conns->tree, &c);
    if (!res)
        return 0.0;
    else
        return res->pdr;
}
