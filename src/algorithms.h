#ifndef _ARBITRATOR_ALGORITHMS_H
#define _ARBITRATOR_ALGORITHMS_H

#include "algorithms/roundrobin.h"
#include "algorithms/ratio.h"
#include "algorithms/dynamicratio.h"
#include "algorithms/leastconnections.h"
#include "nodes.h"          // return node_t


//FIXME: possibly change "char *" to int, cuz inet_ntop, inet_pton??
//       con: IPv6
/*
 * Performs the actual load-balancing of server internet-addressing.
 */
typedef node_t *(*algorithm_func)(const char *);

/*
 * Sets up whatever algorithm structures or variables that may be
 * necessary to perform the algorithm function.
 */
typedef int (*algorithm_init)();

typedef enum {
    DYNAMICRATIO,
    LEASTCONNECTIONS,
    RATIO,
    ROUNDROBIN
} algorithm_name;

/*
 * Structure that holds the necessary information to setup a
 * load-balnacing.
 *
 * XXX: Function naming:    `algorithm_<method>'
 *      Initialize naming:  `init_<method>'
 */
typedef struct Algorithm {
    algorithm_name name;
    algorithm_func function;
    algorithm_init initialize;
} algorithm_t, *algorithm_ptr;


extern algorithm_t current_algorithm;
/*
 * Sets the current_algorithm variable. This is the algorithm that will be
 * used for load balancing. Exactly 1 algorithm must be active at a time.
 */
int set_algorithm(algorithm_name);

#endif /* _ARBITRATOR_ALGORITHMS_H */
