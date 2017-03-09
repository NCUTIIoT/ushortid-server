#ifndef _SELECTOR_H
#define _SELECTOR_H

#include "population.h"

void selector_roulette(population_t *p, scored_chrom_t **p1, scored_chrom_t **p2);
Chromo_t *crossover(scored_chrom_t *p1, scored_chrom_t *p2);

#endif