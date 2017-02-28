#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "ushortid-server.h"

int main()
{
    void *s;
    pthread_t ushortid_svr;
    unsigned int ushortid_svr_port = 15004;
    int r;

    r = pthread_create(&ushortid_svr, NULL, ushortid_server_entry, &ushortid_svr_port);
    if (r)
    {
        fprintf(stderr, "[ERROR] pthread_create failed(%i)\n", r);
        return 1;
    }

    r = pthread_join(ushortid_svr, &s);
    if (r)
    {
        fprintf(stderr, "[ERROR] pthread_join failed(%i)\n", r);
        return 1;
    }
    return 0;
}
