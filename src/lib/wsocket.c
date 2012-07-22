/* wsocket.c
 *
 * This module is strictly for convenience. It is derived from the book
 * "Unix Network Programming" by Richard Stevens.
 *
 */

#include "wsocket.h"

int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int n;

again:
    if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
        if (errno == ECONNABORTED)
#endif
            goto again;
        else
            perror("accept error");
    }
    return n;
}


void
Bind(int fd, struct sockaddr *sa, socklen_t salen)
{
    if (bind(fd, sa, salen) < 0)
        perror("bind error");
}


void
Connect(int fd, struct sockaddr *sa, socklen_t salen)
{
    if (connect(fd, sa, salen) < 0)
        perror("connect error");
}


void
Inet_pton(int family, const char *strptr, void *addrptr)
{
    int n;

    if ( (n = inet_pton(family, strptr, addrptr)) < 1)
        perror("inet_pton error");
}


void
Listen(int fd, int backlog)
{
    char *ptr;

    /*4can override 2nd argument with environment variable */
    if ( (ptr = getenv("LISTENQ")) != NULL)
        backlog = atoi(ptr);

    if (listen(fd, backlog) < 0)
        perror("listen error");
}


int
Socket(int family, int type, int protocol)
{
    int n;

    if ( (n = socket(family, type, protocol)) < 0)
        perror("socket error");
    return n;
}
