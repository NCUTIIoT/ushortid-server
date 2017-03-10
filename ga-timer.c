#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "population.h"
#include "input.h"
#include "simulation.h"
#include "uhurricane-listener.h"
#include "ga-timer.h"
#include "sendtomote.h"

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
    char addrbuf[64], msgbuf[16];
    SimData_t SData;
    WirelessNodes_t wnodes;
    WirelessNode_t *node1, *node2;
    Conns_t conns;
    population_t *pop;
    Chromo_t *maxScored;
    const char *configFilename = 0;
    unsigned int i;

    wnode_init(&wnodes);
    conn_init(&conns);

    handleReadError(MOTES_BIN_FILENAME, file2wnodes(MOTES_BIN_FILENAME, &wnodes));
    handleReadError(MOTES_CONNECTIONS_FILENAME, file2conns(MOTES_CONNECTIONS_FILENAME, &wnodes, &conns));

    SData.wnodes = &wnodes;
    SData.conns = &conns;
    input_getConfig(&SData, configFilename);
    simulation_start(&SData, 0);

    pop = SData.result;
    printChrom((maxScored = population_maxScoreChrom(pop)));
    if (maxScored)
    {
        for (i = 0; i < maxScored->length; i += 1)
        {
            node1 = wnode_findByNid(i, &wnodes);
            if (node1->isRoot != 0)
                continue; // DAGRoot
            node2 = wnode_findByNid((maxScored->p)[i], &wnodes);
            sprintf(addrbuf, "bbbb::%x%x:%x%x:%x%x:%x%x", (node1->addr)[0], (node1->addr)[1], (node1->addr)[2], (node1->addr)[3], (node1->addr)[4], (node1->addr)[5], (node1->addr)[6], (node1->addr)[7]);
            memset(msgbuf, 0, 8);
            memcpy(msgbuf + 8, node2->addr, 8);
            if (sendtomote_send(addrbuf, msgbuf, 15003, 16) != 0)
            {
                fprintf(stderr, "[Warning] Send failed.\n");
            }
        }
    }

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
