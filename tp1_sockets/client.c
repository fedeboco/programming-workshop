#define _POSIX_C_SOURCE 200112L

#include <ctype.h>
#include <string.h>
#include "protocol.h"
#include "client.h"

void client_read_command(client_t *self, char *command){
    int c;

    c = fgetc(stdin);
    if (c == EOF){
        self->status = CLIENT_EOF_REACHED;
    } else {
        ungetc(c, stdin);
        if (fgets(command, READ_BUFF_MAX, stdin) == NULL)
            self->status = CLIENT_ENDGAME;
    }   
}

void client_receive_string(client_t *self, socket_t * skt, char *board){
    protocol_receive_string(skt, board);
}

void client_send_command(client_t *self, socket_t * skt){
    protocol_send_command(skt, self);
}

/*Esta función se asegura de validar correctamente la entrada por
teclado para que los comandos se encuentren correctamente
restringidos y debido a eso posee una considerable extensión.*/
void client_command_process(client_t *self, char *command){
    char * partial_command;
    char * str_ptr;
    char delim[] = DELIMITER;
    char aux_command[MAX_INPUT];
    memset(aux_command,1,MAX_INPUT);

    if (self->status != CLIENT_EOF_REACHED){
        snprintf(aux_command, sizeof(aux_command), "%s", command);
        partial_command = strtok_r(aux_command, delim, &str_ptr);
        if (!strcmp(partial_command,"put")){
            self->command = 'P';
            partial_command = strtok_r(str_ptr, delim, &str_ptr);
            if (strlen(partial_command)==1 && isdigit(partial_command[0])){
                self->value = (uint8_t)partial_command[0] - 48;
                partial_command = strtok_r(str_ptr, delim, &str_ptr);
                if (!strcmp(partial_command,"in")){
                    partial_command = strtok_r(str_ptr, delim, &str_ptr);
                    if (strlen(partial_command)==4  &&
                        isdigit(partial_command[0]) &&
                        partial_command[0]!='0'     &&
                        partial_command[1]==','     &&
                        isdigit(partial_command[2]) &&
                        partial_command[2]!='0'     &&
                        partial_command[3]=='\n'){
                            self->row = (uint8_t)partial_command[0] - 48;
                            self->col = (uint8_t)partial_command[2] - 48;
                            self->status = CLIENT_COMMAND_VALID;
                        } else {
                            fprintf(stderr, "%s", ERROR_INDEX);
                        }
                } else {
                    fprintf(stderr, "%s", ERROR_INVALID_PUT_COMMAND);
                    self->status = CLIENT_COMMAND_INVALID;
                }
            } else {
                self->status = CLIENT_COMMAND_INVALID;
                fprintf(stderr, "%s", ERROR_INPUT_VAL);
            }
        } else if (!strcmp(command,"verify\n")) {
            self->command = 'V';
            self->status = CLIENT_COMMAND_VALID;
        } else if (!strcmp(command,"get\n")) {
            self->command = 'G';
            self->status = CLIENT_COMMAND_VALID;
        } else if (!strcmp(command,"reset\n")) {
            self->command = 'R';
            self->status = CLIENT_COMMAND_VALID;
        } else if (!strcmp(command,"exit\n")) {
            self->command = 'E';
            self->status = CLIENT_COMMAND_VALID;
        } else if (command[0] == EOF) {
            self->command = 'E';
            self->status = CLIENT_COMMAND_VALID;
        } else {
            fprintf(stderr, "%s", ERROR_INVALID_COMMAND);
            self->status = CLIENT_COMMAND_INVALID;
        }
    }
}

void client_connect(client_t *self, socket_t *skt, char* host, char* port){
    skt_connect(skt, host, port);
    if (skt->status == SKT_CONNECTED)
        self->status = CLIENT_CONNECTED;
}
