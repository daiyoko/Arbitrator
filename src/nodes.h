#ifndef _ARBITRATOR_NODES_H
#define _ARBITRATOR_NODES_H

#include <pthread.h>


/*
 * nodes
 */
pthread_mutex_t nodes_lock;

typedef struct Node {
    int ip; // stored as numeric
    unsigned int rtt;   // round-trip-time

    // ... more fields?

    struct Node *next;  // FIXME: delete, use generic linked-list
} node_t;

node_t *node_head;

typedef int (add_remove_node_routine) (char *);
int add_node(char *);
int remove_node(char *);
int show_nodes(void);

#endif /* _ARBITRATOR_NODES_H */
