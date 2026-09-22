#ifndef INTERFACES_H
#define INTERFACES_H

/* TODO: Allow user to change this */
#define CHUNK_SIZE 4096

int send_chunk(const unsigned char data[CHUNK_SIZE], int i);
int recv_chunk(unsigned char out[CHUNK_SIZE], int i);

#endif