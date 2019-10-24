#ifndef SERVERH
#define SERVERH

#include <stdbool.h>
#include "socket.h"
#include "board.h"
#include "protocol.h"

#define BOARD_SENT_SIZE 723
#define BOARD_ROW_SEP_CELL "U===========U===========U===========U\n"
#define BOARD_ROW_SEP "U---+---+---U---+---+---U---+---+---U\n"
#define BOARD_ROW "U   |   |   U   |   |   U   |   |   U\n"
#define BOARD_COL_SEP_CELL "U"
#define BOARD_COL_SEP "|"
#define CH_PER_ROW 38
#define ROWS 19

#define MSG_NON_MODIFIABLE_CELL "La celda indicada no es modificable\n"
#define MSG_ERROR_VERIFY "ERROR\n"
#define MSG_OK_VERIFY "OK\n"
#define MSG_INVALID_CMD "Se ingresó un comando invalido "
#define MSG_FAILED_CONNECTION "Conexion fallida\n"
#define MSG_CLIENT_CONNECTED "Cliente conectado en puerto "

typedef enum{
    SERVER_OK,
    SERVER_COMMAND_INVALID,
    SERVER_DISCONNECTED_CLIENT
} server_status_t;

typedef struct{
    char command;
    uint8_t row;
    uint8_t col;
    uint8_t value;
    server_status_t status;
} server_t;

void server_send_board(server_t *self, socket_t *, board_t *);
void server_receive_command(server_t *self, socket_t *);
void server_process_command(server_t *self, board_t *, socket_t *);
void server_board_read(server_t *self, board_t *board);
void server_wait_user(server_t *self, socket_t *skt, char *port);
void server_destroy(server_t *server, socket_t *skt);
void server_format_board(char *board);

#endif
