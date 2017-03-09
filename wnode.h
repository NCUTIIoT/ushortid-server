#ifndef _WNODE_H
#define _WNODE_H

#define NUM_TREES 2
#define ADDR_LENGTH 8

#include "avl.h"

typedef struct
{
    AVL_TREE *trees[NUM_TREES];
} WirelessNodes_t;

typedef struct
{
    unsigned char addr[ADDR_LENGTH]; // Required
    unsigned int nid;                // Overwrotten and returned by wnode_addNode
    unsigned char isRoot;            // Required
} WirelessNode_t;

void wnode_init(WirelessNodes_t *wnodes);
void wnode_destroy(WirelessNodes_t *wnodes);

unsigned int wnode_addNode(const WirelessNode_t *wnode, WirelessNodes_t *wnodes); /* will allocate space and copy it before add to trees */
WirelessNode_t *wnode_findByAddr(const unsigned char *addr, WirelessNodes_t *wnodes);
WirelessNode_t *wnode_findByNid(unsigned int nid, WirelessNodes_t *wnodes);

unsigned int wnode_getNodesCount(WirelessNodes_t *wnodes);

void addr2str(char *buf, WirelessNode_t *wnode); //buf[2*ADDR_LENGTH + (ADDR_LENGTH-1)]

#endif
