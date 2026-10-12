#ifndef RELAY_H
#define RELAY_H

/* 
Runs the relay server until it is killed.
`port` is the TCP port to listen on
Returns a non-zero exit code if it could not start
*/
int run_relay(const char *port);

#endif