#ifndef PROTOCOLH
#define PROTOCOLH

#define COMMAND_SIZE 2
#define COMMAND_PAMS_SIZE 5
#define HEADER_LEN 4

#include "socket.h"
#include "client.h"
#include "server.h"

typedef enum {
    PROTOCOL_OK,
    PROTOCOL_ERROR
} protocol_status_t;

typedef struct {
    char command[COMMAND_SIZE];
    char command_pams[COMMAND_PAMS_SIZE];
    protocol_status_t status;
} protocol_t;

void protocol_send_command(socket_t*, client_t*);
void protocol_receive_command(socket_t*, server_t*);
void protocol_send_string(socket_t *, char*);
void protocol_receive_string(socket_t *, char*);

#endif
