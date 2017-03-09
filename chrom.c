#include "chrom.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void chrom_init(Chromo_t *c, unsigned int length)
{
    c->length = length;
    c->p = malloc(sizeof(*(c->p)) * length);
}

void chrom_destroy(Chromo_t *c)
{
    free(c->p);
}

double reliability(double pdr, unsigned int nTransmissionTimes)
{
    return 1.0 - pow(1.0 - pdr, nTransmissionTimes);
}

static double _recursive_pathScore(Chromo_t *c, Conns_t *conns, WirelessNodes_t *wnodes, int *loop, unsigned int index, unsigned int nTransmissionTimes, unsigned int level)
{
    double pdr;
    WirelessNode_t *wn1, *wn2;

    if (level > wnode_getNodesCount(wnodes))
    {
        *loop = 1;
        return 0; //Loop
    }

    wn1 = wnode_findByNid(index, wnodes);
    if (!wn1)
    {
        fprintf(stderr, "[CRITIAL ERROR] _recursive_pathScore in %s at %u\n", __FILE__, __LINE__);
        abort();
    }
    if (wn1->isRoot)
        return 1; //DAG Root

    wn2 = wnode_findByNid((c->p)[index], wnodes);
    if (!wn2)
    {
        fprintf(stderr, "[CRITIAL ERROR] _recursive_pathScore in %s at %u\n", __FILE__, __LINE__);
        abort();
    }

    pdr = conn_getPdr(conns, wn1->nid, wn2->nid);
    return reliability(pdr, nTransmissionTimes) * _recursive_pathScore(c, conns, wnodes, loop, (c->p)[index], nTransmissionTimes, level + 1);
}

double fitness_pathScore(Chromo_t *c, Conns_t *conns, WirelessNodes_t *wnodes, unsigned int index, unsigned int nTransmissionTimes)
{
    double s;
    int loop = 0;

    s = _recursive_pathScore(c, conns, wnodes, &loop, index, nTransmissionTimes, 0);
    if (loop)
        return -1.0;
    else
        return s;
}

double fitness_score(Chromo_t *c, Conns_t *conns, WirelessNodes_t *wnodes, unsigned int nTransmissionTimes)
{
    double s = 0.0, r;
    unsigned int i, n;

    n = c->length;
    if (!n)
        return 0.0;
    for (i = 0; i < n; i += 1)
    {
        r = fitness_pathScore(c, conns, wnodes, i, nTransmissionTimes);
        if (r == -1.0)
            return -1.0;
        else
            s += r;
    }

    return s / n;
}

void printChrom(Chromo_t *a)
{
    unsigned int j;

    printf("Chrom> ");
    if (a)
    {
        for (j = 0; j < a->length; j += 1)
        {
            printf("%u ", (a->p)[j]);
        }
    }
    else
    {
        printf("(NULL)");
    }

    printf("\n");
}