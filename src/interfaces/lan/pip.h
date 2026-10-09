#ifndef PIP_H
#define PIP_H

#include <stddef.h>

extern char public_ip[32];

/*
Writes N bytes of the current public ip address to DEST.
DEST will be left untouched if the call fails.
Returns 0 on success and 1 for errors.
*/
const char *get_public_ip(void);

#endif