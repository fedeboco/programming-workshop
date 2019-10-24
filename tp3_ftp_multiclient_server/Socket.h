#ifndef TP3_SOCKET_H
#define TP3_SOCKET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <vector>
#include "common_error.h"

#define WAITING_CLIENTS 4  // max clients number

#define ERROR_ADDRINFO_SERVER "Error getting server's address info"
#define ERROR_SOCKET_ENDPOINT "Error creating socket's endpoint"
#define ERROR_CONNECTING "Error connecting socket to server"
#define ERROR_WAKING_SOCKET "Error setting socket to alive"
#define ERROR_CONFIGURING_CLIENT_SOCKET "Error using setsockopt()"
#define ERROR_SETTING_LISTEN "Error setting socket as listening."
#define ERROR_BINDING "Error making bind."
#define ERROR_SOCKET_ACCEPTOR "Error accepting socket."
#define ERROR_SHUTDOWN "Error on socket shutdown"
#define ERROR_RECEPTION "Error in reception of socket"
#define ERROR_SENDING "Error in sending of socket"

typedef struct addrinfo addrinfo_t;

/* Handles the connection between server and
 * client. Implements the send/receive logic */
class Socket {
 protected:
    int socket_id;
    bool is_alive = false;

 public:
    explicit Socket(char *, char *);  // client
    explicit Socket(char * port);  // server
    explicit Socket(int id);
    Socket();

    void send(int, char *, int);
    void bind(addrinfo_t *res);
    void shutdown();
    void listen();
    int receive(int, char *, int);
    int accept();
    int get_id();
    bool is_accepted();
    bool is_dead();
    std::ostream& operator<<(std::ostream&) const;
};

#endif  // TP3_SOCKET_H
