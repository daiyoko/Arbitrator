/* services.c
 */
#include <netinet/in.h>     // sockaddr_in
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // memset
#include <sys/socket.h>

#include "connections.h"
#include "services.h"
#include "lib/wsocket.h"


service_t services;


/*
 * This is a grotesque hack... please remove if possible!
 *
 * Based on some research...
 *
 * On a socket "write" call, if the file-descriptor cannot be written,
 * then a SIGPIPE signal may be raised. This will cause the program to
 * halt with a rc of 141.
 *
 * Instead, we catch the signal and print a message for now...
 */
void
sig_sigpipe(int signo)
{
    DEBUG("ENCOUNTERED SIGNAL SIGPIPE!!!\n");
}


/*
 * @arg service_type: Enumerater of service_e.
 * @arg port: Port of service.
 */
int
start_service(service_e service_type, short port)
{
    struct Service *service;

    if ( (service = check_service(service_type, port)) == NULL) {
        service = add_service(service_type, port);
        INFO("STARTED service %s:%hu", service_str(service_type), port);
    }
    pthread_create(&service->thread, NULL, run_service_thread, service);
}


/*
 * @arg service_type: Enumerater of service_e.
 * @arg port: Port of service.
 */
int
stop_service(service_e service_type, short port)
{
    struct Service *service;
    char *service_s;

    list_for_each_entry(service, &services.list, list) {
        if (service->name == service_type && service->port == port) {
            pthread_cancel(&service->thread);
            free(service);
            list_del(&service->list);
            INFO("STOPPED service %s:%hu", service_str(service_type),
                 port);
            return 1;
        }
    }
    INFO("Cannot stop service: port=%hu", port);
    return 0;
}


/*
 * @arg service_type: Enumerater of service_e.
 * @arg port: Port of service.
 */
struct Service *
check_service(service_e service_type, short port)
{
    struct Service *service;

    list_for_each_entry(service, &services.list, list) {
        if (service->name == service_type && service->port == port) {
            return service;
        }
    }
    return NULL;
}


/*
 * @arg service_type: Enumerater of service_e.
 * @arg port: Port of service.
 * @return service: Pointer to the added service.
 *
 * Creates "service_t" corresponding to type/port.
 * Adds "service_t" to "services" list.
 */
struct Service *
add_service(service_e service_type, short port)
{
    struct Service *service;
    service = malloc(sizeof(struct Service));

    service->name = service_type;
    service->port = port;
    service->connection_table = HASH_CREATE();
    switch (service_type) {
        case (FTP):
            //service->handle = ftp_request_thread;
            break;
        case (HTTP):
            service->handle = http_request_thread;
            break;
        case (HTTPS):
            //service->handle = https_request_thread;
            break;
        default:
            INFO("Unknown service type\n");
    }
    list_add(&(service->list), &(services.list));
    return service;
}


void
show_services(void)
{
    struct Service *service;

    list_for_each_entry(service, &services.list, list) {
        if (service != NULL) {
            INFO("Service: %s:%hu", service_str(service->name),
                 service->port);
        }
    }
}


const char *
service_str(service_e service_type)
{
    switch (service_type) {
        case (FTP):
            return "FTP";
        case (HTTP):
            return "HTTP";
        case (HTTPS):
            return "HTTPS";
        default:
            return "unknown";
    }
}


/*
 * @arg service: Pointer to struct Service.
 *
 * @var listendf:   arbitrator socket descriptor
 * @var clientfd:   client socket descriptor
 * @var servaddr:   server address
 *
 */
void *
run_service_thread(void *service)
{
    int listendf, clientfd;
    struct sockaddr_in client_addr, proxy_addr;
    socklen_t clilen;
    struct Service *service_ptr;
    struct Connection *connection_ptr;

    char *buffer[15];   // xxx.xxx.xxx.xxx  # update for IPv6?!

    service_ptr = (struct Service *)service;
    short port = service_ptr->port;

    // setup proxy_addr
    listendf = Socket(AF_INET, SOCK_STREAM, 0);
    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    proxy_addr.sin_port = htons(port);

    // bind proxy_addr
    Bind(listendf, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr));
    Listen(listendf, LISTENQ);

    signal(SIGPIPE, sig_sigpipe);

    for (;;) {
        clilen = sizeof(client_addr);
        clientfd = Accept(listendf, (struct sockaddr *)&client_addr,
                          &clilen);

        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, buffer,
                  sizeof(buffer));
        connection_ptr = connection_lookup(service_ptr->connection_table,
                                           buffer, port);
        pthread_mutex_lock(&connection_ptr->in_progress);
        connection_ptr->clientfd = clientfd;

        pthread_create(&connection_ptr->thread, NULL,
                       service_ptr->handle,
                       connection_ptr);
    }
}


/*
 * @arg service: Pointer to struct Connection.
 *
 * NOTE: This class is responsible for cleaning up client file-descriptors
 *       since we are in a thread and joining does not seem practical
 *       in this particular scenario.
 */
void *
http_request_thread(void *connection_ptr)
{
    int n;
    char buf[MAXLINE_TCP];
    struct sockaddr_in serveraddr;
    struct Connection *connection;
    int serverfd; //move into connection struct?!

    connection = (struct Connection *)connection_ptr;

    DEBUG("HTTP REQUEST %s:%d\n",
          inet_ntoa(connection->node->ip),
          connection->port);//LOG

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(connection->port);
    Inet_pton(AF_INET, inet_ntoa(connection->node->ip),
              &serveraddr.sin_addr);

    serverfd = Socket(AF_INET, SOCK_STREAM, 0);
    Connect(serverfd, (struct sockaddr *)&serveraddr,
            sizeof(serveraddr));
    DEBUG("connected to %s\n", inet_ntoa(serveraddr.sin_addr));//LOG
    /*
     * client       =>      proxy   =>      server
     * [clientfd]                           [serverfd]
     *
     * REQUEST "GET /"      ...
     */
    memset(buf, 0, sizeof(char) * MAXLINE_TCP);
    read(connection->clientfd, buf, MAXLINE_TCP);
    DEBUG("======================");
    DEBUG("client->proxy->server:");
    DEBUG("======================\n%s", buf);
    write(serverfd, buf, strlen(buf));

    /*
     * client       <=      proxy   <=      server
     * [clientfd]                           [serverfd]
     *
     *                      ...             RESPONSE "HTTP/1.0 200 OK"
     */
    memset(buf, 0, sizeof(char) * MAXLINE_TCP);
    while ( (n = read(serverfd, buf, MAXLINE_TCP)) > 0) {
        DEBUG("=====================");
        DEBUG("client<-proxy<-sever:");
        DEBUG("=====================\n%s", buf);
        write(connection->clientfd, buf, n);
    }
    DEBUG("FINISHED PROCESSING REQUEST");

    close(serverfd);
    // close here, since we are in a thread, only
    // this thread knows when the request is finished.
    close(connection->clientfd);
    pthread_mutex_unlock(&connection->in_progress);

    pthread_exit(NULL);
}
