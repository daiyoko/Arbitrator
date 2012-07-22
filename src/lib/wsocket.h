#ifndef _ARBITRATOR_WSOCKET_H
#define _ARBITRATOR_WSOCKET_H


#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>


int Accept(int, struct sockaddr *, socklen_t *);
void Bind(int, struct sockaddr *, socklen_t);
void Connect(int, struct sockaddr *, socklen_t);
void Listen(int, int);
int Socket(int, int, int);


#endif /* _ARBITRATOR_WSOCKET_H */
