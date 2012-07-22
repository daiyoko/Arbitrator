#ifndef _ARBITRATOR_SERVICES_H
#define _ARBITRATOR_SERVICES_H

#include <pthread.h>
#include "lib/hash.h"
#include "lib/list.h"
#include "lib/logging.h"


typedef enum {
    FTP,
    HTTP,
    HTTPS
} service_e;

typedef struct Service {
    short port;
    service_e name;

    // thread that runs the handle
    pthread_t thread;

    // function for handling specific service (HTTP, FTP, etc)
    void *(*handle)(void *);

    // holds all connections
    hashtable_t *connection_table;

    // This is used to keep a list of all the services
    // available so that it is easily traversable.
    struct list_head list;

    // boolean active; //TODO?
} service_t;

// LISTENQ could be derived from SOMAXCON in <sys/socket.h>, but many
// kernels still #define it as 5, while actually supporting many more
#define LISTENQ     1024

// dunno what this should really be...
// maybe determine dynamically using Socket API?
#define MAXLINE_TCP 4096

/*
 * List of all services added using "add_services".
 * Traverse using "struct list_head".
 */
extern service_t services;

void sig_sigpipe(int);

typedef int (start_stop_service_routine) (service_e, short);
int start_service(service_e, short);
int stop_service(service_e, short);
service_t *check_service(service_e, short);
service_t *add_service(service_e, short);

void show_services(void);
const char *service_str(service_e);

void *run_service_thread(void *);
void *http_request_thread(void *);

#endif /* _ARBITRATOR_SERVICES_H*/
