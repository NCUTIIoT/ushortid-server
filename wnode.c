#include "wnode.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int _cmp_t0(void *a, void *b)
{
    WirelessNode_t *c, *d;

    c = (WirelessNode_t *)a;
    d = (WirelessNode_t *)b;

    if (c->nid > d->nid)
        return 1;
    else if (c->nid < d->nid)
        return -1;
    else
        return 0;
}

static int _cmp_t1(void *a, void *b)
{
    WirelessNode_t *c, *d;

    c = (WirelessNode_t *)a;
    d = (WirelessNode_t *)b;

    return memcmp(c->addr, d->addr, sizeof(c->addr));
}

static void _releaser(void *a)
{
    free(a);
}

void wnode_init(WirelessNodes_t *wnodes)
{
    (wnodes->trees)[0] = AVL_Create(_cmp_t0, _releaser);
    (wnodes->trees)[1] = AVL_Create(_cmp_t1, 0);
}
void wnode_destroy(WirelessNodes_t *wnodes)
{
    AVL_Destroy((wnodes->trees)[0]);
    AVL_Destroy((wnodes->trees)[1]);
}

unsigned int wnode_addNode(const WirelessNode_t *wnode, WirelessNodes_t *wnodes)
{
    WirelessNode_t *ptr;

    ptr = wnode_findByAddr(wnode->addr, wnodes);
    if (ptr)
        return 0;

    ptr = malloc(sizeof(*ptr));
    memcpy(ptr, wnode, sizeof(*ptr));
    ptr->nid = AVL_Count((wnodes->trees)[0]);
    AVL_Insert((wnodes->trees)[0], ptr);
    AVL_Insert((wnodes->trees)[1], ptr);
    return ptr->nid;
}

WirelessNode_t *wnode_findByAddr(const unsigned char *addr, WirelessNodes_t *wnodes)
{
    WirelessNode_t key;

    memcpy(key.addr, addr, ADDR_LENGTH);
    return (WirelessNode_t *)AVL_Retrieve((wnodes->trees)[1], &key);
}

WirelessNode_t *wnode_findByNid(unsigned int nid, WirelessNodes_t *wnodes)
{
    WirelessNode_t key;

    key.nid = nid;
    return (WirelessNode_t *)AVL_Retrieve((wnodes->trees)[0], &key);
}

unsigned int wnode_getNodesCount(WirelessNodes_t *wnodes)
{
    return AVL_Count((wnodes->trees)[0]);
    //return _g_nid;
}

void addr2str(char *buf, WirelessNode_t *wnode)
{
    unsigned short a[ADDR_LENGTH];
    unsigned int i;

    for (i = 0; i < ADDR_LENGTH; i += 1)
        a[i] = (wnode->addr)[i];

    sprintf(buf, "%2hx:%2hx:%2hx:%2hx:%2hx:%2hx:%2hx:%2hx", a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
}
