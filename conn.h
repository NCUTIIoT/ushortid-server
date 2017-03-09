#ifndef _CONN_H
#define _CONN_H

#include "wnode.h"

typedef struct {
    AVL_TREE *tree;
} Conns_t;

void conn_init(Conns_t *conns);
void conn_destroy(Conns_t *conns);

int conn_setPdr(Conns_t *conns, WirelessNodes_t *wnodes, unsigned char *addr1, unsigned char *addr2, double pdr);
double conn_getPdr(Conns_t *conns, unsigned int id1, unsigned int id2);

#endif
