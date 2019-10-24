#ifndef SOCKETH
#define SOCKETH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>

#define WAITING_CLIENTS 1
#define MSG_NEW_CLIENT "Nuevo cliente"

typedef enum {
    SKT_CONNECTION_FAILURE,
    SKT_CREATE_FAILURE,
    SKT_RECEPTION_FAILURE,
    SKT_RECEPTION_SUCCESS,
    SKT_RECEPTION_END,
    SKT_SEND_FAILURE,
    SKT_SEND_SUCCESS,
    SKT_CONNECTED,
    SKT_SHUTDOWN
} socket_status_t;

typedef struct {
    int skt;
    socket_status_t status;
} socket_t;

typedef struct addrinfo addrinfo_t;

void skt_connect(socket_t *self, char *, char *);
void skt_create(socket_t *self, char *);
void skt_destroy(socket_t *self);
void skt_accept(socket_t *self);
void skt_send(socket_t *self, int, char *);
void skt_listen(socket_t *self);
void skt_bind(socket_t *self, addrinfo_t *);
void skt_setsockoptions(socket_t *self, addrinfo_t *);
int skt_receive(socket_t *self, int, char *);

#endif
