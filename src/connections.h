#ifndef _ARBITRATOR_CONNECTIONS_H
#define _ARBITRATOR_CONNECTIONS_H

#include <sys/time.h>

#include "nodes.h"
#include "lib/hash.h"
#include "lib/logging.h"


typedef struct Connection {
    struct Node *node;
    int clientfd;
    int serverfd;
    short port;

    // used to handle <service>_request_thread
    pthread_t thread;

    // The issues is that if a client makes 2
    // requests before we have finished handling
    // the first one, then we want to make sure
    // we only handle the request 1 time! Else
    // we might use file-descriptors already in
    // use, and bad things may ensue...
    pthread_mutex_t in_progress;

    // TODO: update to prune stale connections?
    struct timeval ttl;
} connection_t;

struct Connection *connection_lookup(hashtable_t*, char *, short);

#endif /* _ARBITRATOR_CONNECTIONS_H */
