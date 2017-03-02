#ifndef _USHORTID_SERVER_H
#define _USHORTID_SERVER_H

void *ushortid_server_entry(void *port_uint_ptr);
const unsigned char *ushortid_server_getAddrById(unsigned short id);

#endif