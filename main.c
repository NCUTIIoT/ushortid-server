#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "ushortid-server.h"
#include "uhurricane-listener.h"
#include "ga-timer.h"

static void launch_thread(pthread_t *outPid, void *(*entry)(void *), unsigned int *port)
{
    int r;

    r = pthread_create(outPid, NULL, entry, port);
    if (r)
    {
        fprintf(stderr, "[ERROR] pthread_create failed(%i)\n", r);
        exit(1);
    }
}

static void wait_thread(pthread_t *outPid)
{
    void *s;
    int r;

    r = pthread_join(*outPid, &s);
    if (r)
    {
        fprintf(stderr, "[ERROR] pthread_join failed(%i)\n", r);
        exit(1);
    }
}

unsigned int nRoots;
unsigned char **roots;
int main(int argc, char **argv)
{
    unsigned int buf[8];
    pthread_t ushortid_svr, uhurricane_listener, ga_timer;
    unsigned int ushortid_svr_port = 15004, uhurricane_listener_port = 15003, ga_timer_interval = 30, i, j;

    nRoots = argc - 1;
    roots = (unsigned char **)malloc(sizeof(*roots) * nRoots);
    for (i = 0; i < nRoots; i += 1)
    {
        roots[i] = (unsigned char *)malloc(8);
        if (sscanf(argv[i + 1], "%x:%x:%x:%x:%x:%x:%x:%x", buf + 0, buf + 1, buf + 2, buf + 3, buf + 4, buf + 5, buf + 6, buf + 7) != 8)
        {
            fprintf(stderr, "[ERROR] '%s' is not a 8-byte address.\n", argv[i + 1]);
            return 1;
        }
        for (j = 0; j < 8; j += 1)
        {
            if (buf[j] > 0xFF)
            {
                fprintf(stderr, "[ERROR] '%s' is not a 8-byte address.\n", argv[i + 1]);
                return 1;
            }
            *(roots[i] + j) = (unsigned char)buf[j];
        }
    }

    launch_thread(&ushortid_svr, ushortid_server_entry, &ushortid_svr_port);
    launch_thread(&uhurricane_listener, uhurricane_listener_entry, &uhurricane_listener_port);
    launch_thread(&ga_timer, ga_timer_entry, &ga_timer_interval);

    wait_thread(&ushortid_svr);
    wait_thread(&uhurricane_listener);
    wait_thread(&ga_timer);

    return 0;
}
