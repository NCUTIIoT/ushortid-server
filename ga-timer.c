#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "population.h"
#include "input.h"
#include "simulation.h"
#include "uhurricane-listener.h"
#include "ga-timer.h"

static int handleReadError(const char *filename, int res)
{
    if (res != 0)
    {
        if (res == 1)
        {
            fprintf(stderr, "[ERROR] Unable to open file \"%s\"\n", filename);
        }
        else if (res == 2)
        {
            fprintf(stderr, "[ERROR] Read error in file \"%s\"\n", filename);
        }
        else
        {
            fprintf(stderr, "[ERROR] Unknown error in file \"%s\"\n", filename);
        }
    }
    return res;
}

static void work()
{
    SimData_t SData;
    WirelessNodes_t wnodes;
    Conns_t conns;
    population_t *pop;
    const char *configFilename = 0;

    wnode_init(&wnodes);
    conn_init(&conns);

    handleReadError(MOTES_BIN_FILENAME, file2wnodes(MOTES_BIN_FILENAME, &wnodes));
    handleReadError(MOTES_CONNECTIONS_FILENAME, file2conns(MOTES_CONNECTIONS_FILENAME, &wnodes, &conns));

    SData.wnodes = &wnodes;
    SData.conns = &conns;
    input_getConfig(&SData, configFilename);
    simulation_start(&SData, 0);

    pop = SData.result;
    printChrom(population_maxScoreChrom(pop));

    population_destroy(pop);
    free(pop);
    conn_destroy(&conns);
    wnode_init(&wnodes);

    return;
}

void *ga_timer_entry(void *seconds_uint_ptr)
{
    unsigned int t = *(unsigned int *)seconds_uint_ptr;
    while (1)
    {
        sleep(t);
        if (uhurricane_listener_writeToFile(MOTES_BIN_FILENAME, MOTES_CONNECTIONS_FILENAME) == 0)
            work();
    }
    return 0;
}
