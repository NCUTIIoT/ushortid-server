#include <stdio.h>
#include <string.h> //for memset
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int uhurricane_listener(unsigned int port)
{
    char buf[256];
    char ipstr[INET6_ADDRSTRLEN];
    struct sockaddr_in6 from;
    ssize_t recvlen;
    socklen_t fromlen;

    int sfd; //socket id
    sfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sfd < 0)
    {
        printf("socket creat erro\n");
        return -1;
    }

    memset(&from, 0, sizeof(struct sockaddr_in6));
    from.sin6_family = AF_INET6;
    from.sin6_port = htons(port);
    from.sin6_addr = in6addr_any;
    bind(sfd, (const struct sockaddr *)&(from), sizeof(from));
    fromlen = sizeof(from);
    while ((recvlen = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen)) != -1)
    {
        inet_ntop(from.sin6_family, &(from.sin6_addr), ipstr, sizeof(ipstr));
        //printf("recvfrom %s, port-%u\n", ipstr, ntohs(from.sin6_port));
    }

    close(sfd);
    return 0;
}

void *uhurricane_listener_entry(void *port_uint_ptr)
{
    unsigned int port = *(unsigned int *)port_uint_ptr;
    uhurricane_listener(port);
    return 0;
}