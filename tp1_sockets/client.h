#ifndef CLIENTH
#define CLIENTH

#include "socket.h"
#include "protocol.h"

#define ERROR_INDEX "Error en los índices. Rango soportado: [1,9]\n"
#define ERROR_INVALID_PUT_COMMAND "Sintaxis: put <numero> in <fila>,<columna>"
#define ERROR_INPUT_VAL "Error en el valor ingresado. Rango soportado: [1,9]\n"
#define ERROR_INVALID_COMMAND "Ingrese:\n-put\n-verify\n-get\n-reset\n-exit\n"
#define DELIMITER " "

#define MAX_INPUT 20
#define READ_BUFF_MAX 50

typedef enum{
    CLIENT_OK,
    CLIENT_COMMAND_VALID,
    CLIENT_COMMAND_INVALID,
    CLIENT_ENDGAME,
    CLIENT_CONNECTED,
    CLIENT_EOF_REACHED,
    CLIENT_INVALID_INDEX
} client_status_t;

typedef struct{
    char command;
    uint8_t row;
    uint8_t col;
    uint8_t value;
    client_status_t status;
} client_t;

void client_read_command(client_t *self, char *command);
void client_command_process(client_t *self, char*);
void client_receive_string(client_t *self, socket_t*, char*);
void client_send_command(client_t*, socket_t*);
void client_connect(client_t *self, socket_t *skt, char*, char*);

#endif
