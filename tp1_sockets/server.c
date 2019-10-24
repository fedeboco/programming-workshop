#include "protocol.h"
#include "server.h"

//Esta función envía la tabla formateada al cliente.
void server_send_board(server_t *self, socket_t *socket, board_t *board){
    char * sb = (char*)malloc(BOARD_SENT_SIZE * sizeof(char));//tabla a enviar
    int j = 0;
    int k = 1;
    char aux;
  
    printf("\n");
    server_format_board(sb);

    for (int i=40; i<=680; i=i+4){
        j++;
        if ( (aux = board_value_get(board, k, j)) != '0' )
            sb[i] = aux;

        //Para el salto de fila.
        if (j==9){
            j=0;
            k++;
            i=i+2+CH_PER_ROW;
        }
    }
    fprintf(stdout, "%s",sb);

    protocol_send_string(socket, sb);
    free(sb);
}

void server_format_board(char *board){
    snprintf(board, BOARD_SENT_SIZE,
            "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
            BOARD_ROW_SEP_CELL,
            BOARD_ROW,
            BOARD_ROW_SEP,
            BOARD_ROW,
            BOARD_ROW_SEP,
            BOARD_ROW,
            BOARD_ROW_SEP_CELL,
            BOARD_ROW,
            BOARD_ROW_SEP,
            BOARD_ROW,
            BOARD_ROW_SEP,
            BOARD_ROW,
            BOARD_ROW_SEP_CELL,
            BOARD_ROW,
            BOARD_ROW_SEP,
            BOARD_ROW,
            BOARD_ROW_SEP,
            BOARD_ROW,
            BOARD_ROW_SEP_CELL);
}

void server_process_command(server_t *self, board_t *board, socket_t *skt){
    switch (self->command)
    {
        case 'P':
            board_put(board, self->row, self->col, self->value);
            if (board->status == BOARD_NON_MODIFIABLE_VALUE){
                protocol_send_string(skt, MSG_NON_MODIFIABLE_CELL);
                board->status = BOARD_OK;
            } else {
                server_send_board(self, skt, board);
            }
            break;
        case 'V':
            board_verify(board);
            if (board->status == BOARD_INVALID){
                protocol_send_string(skt, MSG_ERROR_VERIFY);
            } else {
                protocol_send_string(skt, MSG_OK_VERIFY);
            }
            break;
        case 'R':
            board_reset(board);
            server_send_board(self, skt, board);
            break;
        case 'G':
            server_send_board(self, skt, board);
            break;
        case 'E':
            self->status = SERVER_DISCONNECTED_CLIENT;
        default:
            protocol_send_string(skt, MSG_INVALID_CMD);
            break;
    }
}

void server_receive_command(server_t *self, socket_t *skt){
    protocol_receive_command(skt, self);
}

void server_board_read(server_t *self, board_t *board){
    board_read(board);
    board->status = BOARD_OK;
}

void server_wait_user(server_t *self, socket_t *skt, char *port){
    skt_create(skt, port);
    skt_accept(skt);
    if (skt->status != SKT_CONNECTED){
        printf("%s", MSG_FAILED_CONNECTION);
    } else {
        printf("%s%s\n", MSG_CLIENT_CONNECTED, port);  
    }
}

void server_destroy(server_t *server, socket_t *skt){
   skt->status = SKT_SHUTDOWN;
   close(skt->skt);    
}
