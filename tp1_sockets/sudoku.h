#ifndef SUDOKUH
#define SUDOKUH

#include "protocol.h"
#include "client.h"
#include "server.h"
#include "socket.h"
#include "board.h"

#define MAX_COMMAND_LENGTH 20
#define RECEIVED_BOARD 800

#define SUDOKU_ERR_SERVER_CMD "Uso: ./tp server <puerto>\n"
#define SUDOKU_ERR_CLIENT_CMD "Uso: ./tp client <host> <puerto>\n"
#define SUDOKU_ERR_CMD "Modo no soportado, el primer parámetro debe ser "
#define SUDOKU_ERR_CMD_OPTIONS "server o client\n"
#define SUDOKU_MSG_SERVER "server"
#define SUDOKU_MSG_CLIENT "client"

typedef enum{
    SUDOKU_OK,
    SUDOKU_ARGS_OK,
    SUDOKU_ARGS_INVALID,
    SUDOKU_ARGS_NULL,
    SUDOKU_ENDGAME
} sudoku_status_t;

typedef struct{
    char mode;
    char * arg1;
    char * arg2;
    sudoku_status_t status;
} sudoku_t;

void sudoku_set_mode(sudoku_t *self, int argc, char *argv[]);
void sudoku_run(sudoku_t *self);
void sudoku_run_server(sudoku_t *self, server_t *, socket_t *, board_t *);
void sudoku_play(sudoku_t *self, client_t *, socket_t *, board_t *);

#endif
