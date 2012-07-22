/* connections.c
 *
 * Unique Connection exists for every combination of client-ip/port
 * sockets.
 *
 */
#include <netinet/in.h>     // sockaddr_in
#include <stdlib.h>
#include <stdio.h>
#include <string.h>         // memset
#include <sys/socket.h>

#include "algorithms.h"
#include "connections.h"
#include "lib/wsocket.h"


/**
 * @param source_ip: Client's IP address in presentation format.
 * @param port: Server's port connection.
 * @return pointer to connection
 *
 * Looks up existing connections between the arbitrator->server.
 * This is done based off of connections between the client->arbitrator.
 *
 *  [client]            [arbitrator]            [server]
 *            ----->
 *            1st(based)                ----->
 *                                      2nd(derive)
 */
struct Connection *
connection_lookup(hashtable_t *connection_table, char *source_ip,
                  short port)
{
    connection_t *connection;

    if (connection = HASH_GET(connection_table, source_ip)) {
        DEBUG("FOUND %s -> %s:%d\n",
               source_ip, inet_ntoa(connection->node->ip),
               connection->port);
        return connection;
    }
    else {
        struct sockaddr_in servaddr;
        node_t *node;

        connection = malloc(sizeof(connection_t));

        node = current_algorithm.function(source_ip);
        connection->node = node;
        connection->port = port;
        pthread_mutex_init(&connection->in_progress, NULL);

        DEBUG("CREATE %s -> %s:%d\n", source_ip,
               inet_ntoa(connection->node->ip),
               connection->port);

        HASH_ADD(connection_table, source_ip, connection);
        return connection;
    }
}
