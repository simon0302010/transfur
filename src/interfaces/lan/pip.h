#ifndef PIP_H
#define PIP_H

#include <stddef.h>

/*
Writes n bytes of the ip to dest.
dest will be left untouched if the call fails.
*/
int get_public_ip(char *dest, size_t n);

#endif