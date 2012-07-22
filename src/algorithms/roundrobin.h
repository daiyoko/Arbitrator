#ifndef _ALGORITHMS_ROUNDROBIN_H
#define _ALGORITHMS_ROUNDROBIN_H

#include <pthread.h>

struct Node *currentnode;
//extern struct Node node_head;

struct Node*algorithm_roundrobin(const char *);
int init_roundrobin();

pthread_mutex_t currentnode_lock;

#endif /* _ALGORITHMS_ROUNDROBIN_H */
