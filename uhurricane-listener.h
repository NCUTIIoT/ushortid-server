#ifndef _UHURRICANE_LISTENER_H
#define _UHURRICANE_LISTENER_H

void *uhurricane_listener_entry(void *port_uint_ptr);
int uhurricane_listener_writeToFile(const char *mfilename, const char *cfilename);

#endif
