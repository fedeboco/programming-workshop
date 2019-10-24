#include "sudoku.h"

//Esta función valida los argumentos de cmdline y setea el modo de juego.
void sudoku_set_mode(sudoku_t *self, int argc, char *argv[]){
    self->status = SUDOKU_ARGS_INVALID;
    if (argv == NULL)
        self->status = SUDOKU_ARGS_NULL;

    if (argc == 3 && !strcmp(argv[1],SUDOKU_MSG_SERVER)){
        self->mode = 'S';
        self->arg1 = argv[2];
        self->status = SUDOKU_ARGS_OK;
    } else if (argc == 4 && !strcmp(argv[1],SUDOKU_MSG_CLIENT)){
        self->mode = 'C';
        self->arg1 = argv[2];
        self->arg2 = argv[3];
        self->status = SUDOKU_ARGS_OK;
    } else if (argc == 2 && !strcmp(argv[1],SUDOKU_MSG_SERVER)){
        printf("%s", SUDOKU_ERR_SERVER_CMD);
    } else if (argc == 2 && !strcmp(argv[1],SUDOKU_MSG_CLIENT)){
        printf("%s", SUDOKU_ERR_CLIENT_CMD);
    } else {
        printf("%s%s", SUDOKU_ERR_CMD, SUDOKU_ERR_CMD_OPTIONS);
    }
}

void sudoku_run(sudoku_t *self){
    socket_t socket;
    board_t board;
    client_t client;
    server_t server;
    client.row = 0;
    client.col = 0;
    client.value = 0;
    memset(board.board, 0, sizeof(board.board));
    memset(board.board_original, 0, sizeof(board.board_original));

    switch ( self->mode ){
        case 'S':
            sudoku_run_server(self, &server, &socket, &board);
            break;
        case 'C':
            sudoku_play(self, &client, &socket, &board);
            break;
    }
}

//Esta función ejecuta el servidor para jugar al sudoku.
void sudoku_run_server(sudoku_t *self, 
                        server_t *server, 
                        socket_t *socket, 
                        board_t *board){
    server_wait_user(server, socket, self->arg1);
    if ( socket->status == SKT_CONNECTED ){
        server_board_read(server, board);
        server_receive_command(server, socket); 
        while ( socket->status == SKT_RECEPTION_SUCCESS ){
            server_process_command(server, board, socket);
            server_receive_command(server, socket);
        }
    }
    if (socket->status == SKT_RECEPTION_END)
        self->status = SUDOKU_ENDGAME;
    server_destroy(server, socket);
}

//Esta función ejecuta el cliente para jugar al sudoku.
void sudoku_play(sudoku_t *self, 
                    client_t *client, 
                    socket_t *socket, 
                    board_t *board){
    char received[RECEIVED_BOARD];
    char client_command[MAX_COMMAND_LENGTH];
    client->command = 'X';

    memset(received, 0, RECEIVED_BOARD);
    memset(client_command, 1, MAX_COMMAND_LENGTH);

    client_connect(client, socket, self->arg1, self->arg2);
    if ( client->status == CLIENT_CONNECTED ){
        while ( client->status != CLIENT_ENDGAME ){
            client->status = CLIENT_COMMAND_INVALID;
            while (client->status == CLIENT_COMMAND_INVALID){
                client->status = CLIENT_OK;
                client_read_command(client, client_command);
                client_command_process(client, client_command);
            }
            if (client->command == 'E' || client->status == CLIENT_EOF_REACHED){
                client->status = CLIENT_ENDGAME;
                self->status = SUDOKU_ENDGAME;
                skt_destroy(socket);
            }
            if (client->status == CLIENT_COMMAND_VALID){
                client_send_command(client, socket);
                client_receive_string(client, socket, received);
            }
        }
    }
}
