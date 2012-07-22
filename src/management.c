/* management.c
 *
 * This module is responsible for creating threads used by the arbitrator.
 * There are two main sets of threads:
 *  I.  init/prompt:
 *      A. initializes arbitrator
 *      B. dynamically accepts commands
 *
 *  II. web proxy:
 *      A. Each service listens on a port
 *          i. Each request spawns a thread to respond.
 *      B. Single thread to map request to a server?
 *          (or is it better to have each thread do this on its own...?)
 *
 */
#include "algorithms.h"
#include "management.h"
#include "nodes.h"
#include "prompt.h"
#include "services.h"


void
init_prompt()
{
    //FIXME: threading problem...
    //start_prompt();
    prompt_user(NULL);
}


void
init_services()
{
    //FIXME: check if something already listening on port?


    //XXX: make sure this makes sense here..
    INIT_LIST_HEAD(&services.list);


    //FIXME: this should be set somewhere...
    set_algorithm(ROUNDROBIN);
    start_service(HTTP, 9877);
    start_service(HTTP, 9878);
    //start_service(HTTP, 80);
    //start_service(HTTPS, 443);
    //start_service(FTP, 21);
}


void
cleanup_services()
{
    stop_service(HTTP, 80);
}


/*
 * Initialized as early as possible with __attibute__((constructor)),
 * but *after* config parsing! (So use priority arg)
 */
void __attribute__((constructor))
init_nodes()
{
    add_node("192.168.1.3");
    //add_node("192.168.1.6");
    //add_node("192.168.1.4");
}


/*
 * Initialized as early as possible with __attibute__((constructor))
 * so that we can use logging as soon as possible.
 */
void __attribute__((constructor))
init_logging()
{
    stream = stdout;
    logging_level = INFO;
}


void
set_logging(FILE *log_stream, loglevel_e log_level)
{
    stream = log_stream;
    logging_level = log_level;

}
