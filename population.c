#include "selector.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int __cmp_f(void *a, void *b)
{
    scored_chrom_t *c, *d;

    c = (scored_chrom_t *)a;
    d = (scored_chrom_t *)b;

    if (c->score > d->score)
        return 1;
    else if (c->score < d->score)
        return -1;
    else
        return memcmp(c->chrom->p, d->chrom->p, c->chrom->length * sizeof(*(c->chrom->p)));
}

static void __releaser(void *ptr)
{
    scored_chrom_t *a;

    a = (scored_chrom_t *)ptr;
    chrom_destroy(a->chrom);
    free(a->chrom);

    free(ptr);
}

void population_init(population_t *population, unsigned int size)
{
    population->s_chroms = AVL_Create(__cmp_f, __releaser);
    population->size = size;
    population->scoreSum = 0.0;
}

void population_destroy(population_t *population)
{
    AVL_Destroy(population->s_chroms);
}

int population_addChrom(population_t *population, scored_chrom_t *sch)
{
    int r = AVL_Insert(population->s_chroms, sch);

    if (r)
        population->scoreSum += sch->score;
    return r;
}

void population_firstGen(population_t *population, WirelessNodes_t *wnodes, Conns_t *conns, unsigned int maxRetransmitTimes)
{
    Chromo_t *ch;
    scored_chrom_t *sch;
    unsigned int i, j, k, l, m, n;

    n = wnode_getNodesCount(wnodes);
    m = population->size;
    for (l = 0; l < m; l += 1)
    {
        ch = malloc(sizeof(*ch));
        sch = malloc(sizeof(*sch));
        chrom_init(ch, n);
        do
        {
            for (i = 0; i < n; i += 1)
            {
                j = (unsigned int)(rand() % n);
                k = 0;
                while (conn_getPdr(conns, i, j) == 0.0 || i == j)
                {
                    j = (j + 1) % n;
                    k += 1;
                    if (k >= n)
                    {
                        //fprintf(stderr, "[ERROR] abandoned node.\n");
                        chrom_destroy(ch);
                        free(ch);
                        free(sch);
                        return;
                    }
                }
                (ch->p)[i] = j;
            }
        } while ((sch->score = fitness_score(ch, conns, wnodes, maxRetransmitTimes)) <= 0.0);
        sch->chrom = ch;
        population_addChrom(population, sch);
    }
}

void population_nextGen(population_t *p, population_t *childGen, WirelessNodes_t *wnodes, Conns_t *conns, unsigned int maxRetransmitTimes)
{
    double mutateRate = 0.05;
    Chromo_t *ch;
    scored_chrom_t *a, *b, *sch;
    unsigned int i, j, k, l, m, up, backup, n = childGen->size, wn = wnode_getNodesCount(wnodes);

    for (i = 0; i < n; i += 1)
    {
        selector_roulette(p, &a, &b);
        if (a == 0 || b == 0)
        {
            //fprintf(stderr, "[ERROR] population_nextGen, parent null\n");
            return;
        }
        ch = crossover(a, b);
        up = ((rand() % 0xFF) << 24) + ((rand() % 0xFF) << 16) + ((rand() % 0xFF) << 8) + ((rand() % 0xFF) << 0);
        if ((up % 1000000) / 1000000.0 <= mutateRate)
        {
            l = (unsigned int)(rand() % wn);
            m = 0;
            backup = (ch->p)[l];
            do
            {
                j = (unsigned int)(rand() % wn);
                k = 0;
                while (conn_getPdr(conns, l, j) == 0.0 || l == j)
                {
                    j = (j + 1) % wn;
                    k += 1;
                    if (k >= wn)
                        break;
                }
                if (k < wn)
                    (ch->p)[l] = j;
                m += 1;
                if (m >= wn)
                    break;
            } while (fitness_score(ch, conns, wnodes, maxRetransmitTimes) <= 0.0);
            if (m >= wn)
                (ch->p)[l] = backup;
        }
        //printChrom(ch);
        sch = malloc(sizeof(*sch));

        sch->chrom = ch;
        sch->score = fitness_score(ch, conns, wnodes, maxRetransmitTimes);
        if (sch->score < a->score || sch->score < b->score)
        {
            i -= 1;
            free(ch);
            free(sch);
            continue;
        }

        population_addChrom(childGen, sch);
    }
}

unsigned int population_size(population_t *population)
{
    return AVL_Count(population->s_chroms);
}

static void _traveserSum(void *ptr, void *param)
{
    scored_chrom_t *a;
    double *d = (double *)param;

    a = (scored_chrom_t *)ptr;
    if (a->score > 0.0)
        *d += a->score;
}

double population_sumScore(population_t *population)
{
    double sum = 0.0;

    AVL_Traverse(population->s_chroms, &sum, _traveserSum);
    return sum;
}

double population_avgScore(population_t *population)
{
    return population_sumScore(population) / AVL_Count(population->s_chroms);
}

static scored_chrom_t *_getMaxScored(population_t *population)
{
    NODE *n;

    if (AVL_Count(population->s_chroms) == 0)
        return 0;

    n = population->s_chroms->root;
    while (n->right)
        n = n->right;

    return n->dataPtr;
}

double population_maxScore(population_t *population)
{
    scored_chrom_t *n = _getMaxScored(population);

    if (n)
        return n->score;
    else
        return -1.0;
}

Chromo_t *population_maxScoreChrom(population_t *population)
{
    scored_chrom_t *n = _getMaxScored(population);

    if (n)
        return n->chrom;
    else
        return 0;
}

typedef struct
{
    double p;
    scored_chrom_t *out;
    int selected;
} rouletteS_t;

static void _traveserP(void *ptr, void *param)
{
    scored_chrom_t *a;
    rouletteS_t *d = (rouletteS_t *)param;

    a = (scored_chrom_t *)ptr;
    //printf("[DEBUG] s = %lf, p = %lf\n", a->score, d->p);
    if (a->score > 0.0)
    {
        if (d->selected == 0)
        {
            if (a->score >= d->p)
            {
                d->p = 0.0;
                d->out = a;
                d->selected = 1;
            }
            else
            {
                d->p -= a->score;
            }
        }
    }
}

scored_chrom_t *population_rouletteSelect(population_t *population, double p)
{
    rouletteS_t w;

    memset(&w, 0, sizeof(w));
    w.p = p * population->scoreSum;
    AVL_Traverse(population->s_chroms, &w, _traveserP);

    return w.out;
}
