#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "simulation.h"

void simulation_start(SimData_t *d, int verbose)
{
    WirelessNodes_t *wnodes = d->wnodes;
    Conns_t *conns = d->conns;
    population_t *pop, *child;
    double thres = d->thres;
    unsigned int maxRetransmitTimes = d->maxRetransmitTimes;
    unsigned int noImprovementThres = d->noImprovementThres;
    unsigned int popSize = d->popSize;
    unsigned int c;

    pop = (population_t *)malloc(sizeof(*pop));
    child = (population_t *)malloc(sizeof(*child));
    population_init(pop, popSize);

    srand(time(0) + clock());
    population_firstGen(pop, wnodes, conns, maxRetransmitTimes);
    if (verbose != 0)
        printf("avg = %lf, max = %lf\n", population_avgScore(pop), population_maxScore(pop));
    c = 0;
    while (population_maxScore(pop) < thres)
    {
        population_init(child, popSize);
        population_nextGen(pop, child, wnodes, conns, maxRetransmitTimes);
        if (population_avgScore(pop) < population_avgScore(child))
        {
            population_destroy(pop);
            memcpy(pop, child, sizeof(*pop));
            c = 0;
        }
        else
        {
            population_destroy(child);
            c += 1;
            if (c >= noImprovementThres)
                break;
        }

        if (verbose != 0)
            printf("avg = %lf, max = %lf\n", population_avgScore(pop), population_maxScore(pop));
    }

    free(child);
    d->result = pop;
}