#ifndef _INPUT_H
#define _INPUT_H

#include "conn.h"
#include "simulation.h"

int file2wnodes(const char *filename, WirelessNodes_t *wnodes);
int file2conns(const char *filename, WirelessNodes_t *wnodes, Conns_t *conns);

int input_getConfig(SimData_t *SData, const char *filename);

#endif