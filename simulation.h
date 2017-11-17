#ifndef _SIM_H
#define _SIM_H

#include "population.h"

typedef struct
{
    double thres;
    WirelessNodes_t *wnodes;
    Conns_t *conns;
    population_t *result;
    unsigned int popSize;
    unsigned int maxRetransmitTimes;
    unsigned int noImprovementThres;
    unsigned int nAvailableSlots;
} SimData_t;

void simulation_start(SimData_t *d, int verbose);

#endif