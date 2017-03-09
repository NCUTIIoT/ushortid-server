#ifndef _CHROM_H
#define _CHROM_H

#include "conn.h"

typedef struct
{
    unsigned int *p;
    unsigned int length;
} Chromo_t;

void chrom_init(Chromo_t *c, unsigned int length);
void chrom_destroy(Chromo_t *c);

double fitness_pathScore(Chromo_t *c, Conns_t *conns, WirelessNodes_t *wnodes, unsigned int index, unsigned int nTransmissionTimes);
double fitness_score(Chromo_t *c, Conns_t *conns, WirelessNodes_t *wnodes, unsigned int nTransmissionTimes);

void printChrom(Chromo_t *a);

#endif
