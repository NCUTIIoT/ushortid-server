#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "ushortid-server.h"
#include "uhurricane-listener.h"

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

static void wair_thread(pthread_t *outPid)
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
char **roots;
int main(int argc, char **argv)
{
    pthread_t ushortid_svr, uhurricane_listener;
    unsigned int ushortid_svr_port = 15004, uhurricane_listener_port = 15003;

    nRoots = argc - 1;
    roots = argv + 1;

    launch_thread(&ushortid_svr, ushortid_server_entry, &ushortid_svr_port);
    launch_thread(&uhurricane_listener, uhurricane_listener_entry, &uhurricane_listener_port);

    wair_thread(&ushortid_svr);
    wair_thread(&uhurricane_listener);

    return 0;
}
