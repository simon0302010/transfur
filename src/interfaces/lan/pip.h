#ifndef PIP_H
#define PIP_H

#include <stddef.h>

/*
Writes N bytes of the current public ip address to DEST.
DEST will be left untouched if the call fails.
Returns 0 on success and 1 for errors.
*/
int get_public_ip(char *dest, size_t n);

#endif