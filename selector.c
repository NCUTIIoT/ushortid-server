#include "selector.h"

#include <stdlib.h>
#include <stdio.h>

void selector_roulette(population_t *p, scored_chrom_t **p1, scored_chrom_t **p2)
{
    scored_chrom_t *a, *b;
    unsigned int up;
    double r;

    *p1 = *p2 = 0;

    up = ((rand() % 0xFF) << 24) + ((rand() % 0xFF) << 16) + ((rand() % 0xFF) << 8) + ((rand() % 0xFF) << 0);
    r = (up % 1000000) / 1000000.0;
    a = population_rouletteSelect(p, r);
    if (!a)
        return;
    up = ((rand() % 0xFF) << 24) + ((rand() % 0xFF) << 16) + ((rand() % 0xFF) << 8) + ((rand() % 0xFF) << 0);
    r = (up % 1000000) / 1000000.0;
    b = population_rouletteSelect(p, r);
    if (!b)
        return;

    *p1 = a;
    *p2 = b;
}

Chromo_t *crossover(scored_chrom_t *p1, scored_chrom_t *p2)
{
    Chromo_t *ch;
    unsigned int i, p, n;

    if (p1->chrom->length != p2->chrom->length)
    {
        fprintf(stderr, "[ERROR] crossover, length not match.\n");
        return 0;
    }

    n = p1->chrom->length;
    if (n == 0)
        return 0;

    p = ((rand() % 0xFF) << 24) + ((rand() % 0xFF) << 16) + ((rand() % 0xFF) << 8) + ((rand() % 0xFF) << 0);
    p %= n - 1;

    ch = malloc(sizeof(*ch));

    chrom_init(ch, n);
    for (i = 0; i <= p; i += 1)
        (ch->p)[i] = (p1->chrom->p)[i];
    for (i = p + 1; i < n; i += 1)
        (ch->p)[i] = (p2->chrom->p)[i];

    return ch;
}