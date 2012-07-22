/* algorithms.c
 *
 * This module is responsible for setting the algorithms for arbitrating
 * requests.
 *
 */
#include <pthread.h>
#include <stdio.h>

#include "algorithms.h"

algorithm_t current_algorithm = {.name = ROUNDROBIN,
                                 .function = algorithm_roundrobin,
                                 .initialize = init_roundrobin};


int
set_algorithm(algorithm_name name)
{
    current_algorithm.name = name;
    switch (current_algorithm.name) {
        case DYNAMICRATIO:
            current_algorithm.function = algorithm_dynamicratio;
            current_algorithm.initialize = init_dynamicratio;
            break;
        case LEASTCONNECTIONS:
            current_algorithm.function = algorithm_leastconnections;
            current_algorithm.initialize = init_leastconnections;
            break;
        case RATIO:
            current_algorithm.function = algorithm_ratio;
            current_algorithm.initialize = init_ratio;
            break;
        case ROUNDROBIN:
            current_algorithm.function = algorithm_roundrobin;
            current_algorithm.initialize = init_roundrobin;
            break;
        default:
            printf("%s: Unknown algorithm name\n", __func__);
            return -1;
    }
    current_algorithm.initialize();
    //TODO: define: TRUE, FALSE, or ERR_T
    return 1;
}
