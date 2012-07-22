#include <pthread.h>
#include <stdio.h>

#include "roundrobin.h"
#include "../nodes.h"
//#include "nodes.h"


//FIXME: possibly change "char *" to int, cuz inet_ntop, inet_pton??
//       con: IPv6
//FIXME: __attribute__((section("foo")));
node_t *
algorithm_roundrobin(const char *source_ip)
{
    pthread_mutex_lock(&currentnode_lock);

    if (currentnode->next == NULL) {
        currentnode = node_head;
    }

    pthread_mutex_unlock(&currentnode_lock);
    //printf("RoundRobin-Pick: %s\n", inet_ntoa(currentnode->ip));//LOG
    return currentnode;
}


/*
 * Sets up whatever algorithm structures or variables that may be
 * necessary to perform the algorithm function.
 */
int
init_roundrobin()
{
    printf("Setup RoundRobin\n");//LOG

    pthread_mutex_init(&currentnode_lock, NULL);

    currentnode = node_head;
    if (currentnode == NULL) {
        printf("No nodes...\n");
    }
}
