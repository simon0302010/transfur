#ifndef BE32_H
#define BE32_H

void put_be32(unsigned char *dst, unsigned long v);

unsigned long get_be32(const unsigned char *src);

#endif