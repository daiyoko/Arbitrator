/* nodes.c
 *
 * This module is responsible for adding/removing backend nodes for 
 * load-balancing.
 *
 */
#include <sys/socket.h>
#include <netinet/in.h>     // INET_ADDRSTRLEN

#include <stdio.h>
#include <stdlib.h>

#include "lib/logging.h"
#include "nodes.h"
#include "services.h"


int
add_node(char *ip_destination)
{
    node_t *new_node = malloc(sizeof(node_t));
    Inet_pton(AF_INET, ip_destination, &new_node->ip);
    new_node->next = node_head;
    node_head = new_node;

    printf("Added node: %s\n", inet_ntoa(node_head->ip));
}


int
remove_node(char *ip_destination)
{
    // current/prevous node
    node_t *cnode, *pnode;
    int remove_ip;

    // Remove "ip_destination" from service->connection_table
    struct Service *service;

    cnode = node_head;
    Inet_pton(AF_INET, ip_destination, &remove_ip);

    // Fence-post problem..
    if (cnode->ip == remove_ip) {
        node_head = cnode->next;
        INFO("Removed Node: %s", inet_ntoa(cnode->ip));
    }
    else {
        for (pnode=cnode, cnode=cnode->next; cnode!=NULL;
             cnode=cnode->next, pnode=pnode->next) {
            if (cnode->ip == remove_ip) {
                pnode->next = cnode->next;
                INFO("Removed Node: %s", inet_ntoa(cnode->ip));
                break;
            }
        }
    }

    service = &services;

    list_for_each_entry(service, &services.list, list) {
        HASH_DELETE(&service->connection_table);
        service->connection_table = HASH_CREATE();
    }
    return 0;
}


int
show_nodes(void)
{
    // current node
    node_t *cnode;

    for (cnode=node_head; cnode!=NULL; cnode=cnode->next) {
        INFO("Node: %s", inet_ntoa(cnode->ip));
    }
}
