#ifndef _ARBITRATOR_MANAGEMENT_H
#define _ARBITRATOR_MANAGEMENT_H

#include "lib/logging.h"


FILE *stream;
int logging_level;


void init_prompt();
void init_services();
void init_nodes();
void init_logging();


void set_logging(FILE *, loglevel_e);


#endif /* _ARBITRATOR_MANAGEMENT_H*/
