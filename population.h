#ifndef _POPULATION_H
#define _POPULATION_H

#include "chrom.h"

typedef struct
{
    Chromo_t *chrom;
    double score;
} scored_chrom_t;

typedef struct
{
    double scoreSum;
    AVL_TREE *s_chroms;
    unsigned int size;
} population_t;

void population_init(population_t *population, unsigned int size);
void population_destroy(population_t *population);

int population_addChrom(population_t *population, scored_chrom_t *sch);
void population_firstGen(population_t *population, WirelessNodes_t *wnodes, Conns_t *conns, unsigned int maxRetransmitTimes, unsigned int nAvailableSlots);
void population_nextGen(population_t *p, population_t *childGen, WirelessNodes_t *wnodes, Conns_t *conns, unsigned int maxRetransmitTimes, unsigned int nAvailableSlots);
unsigned int population_size(population_t *population);
double population_sumScore(population_t *population);
double population_avgScore(population_t *population);
double population_maxScore(population_t *population);
Chromo_t *population_maxScoreChrom(population_t *population);

scored_chrom_t *population_rouletteSelect(population_t *population, double p);

#endif