#include "protocol.h"

void protocol_send_command(socket_t *socket, client_t *command){
    char aux = (char)(command->command);

    skt_send(socket, 1, &aux);
    if (aux == 'P'){
        aux = (char)(command->row);
        skt_send(socket, 1, &aux);
        aux = (char)(command->col);
        skt_send(socket, 1, &aux);
        aux = (char)(command->value);
        skt_send(socket, 1, &aux);
    } 
}

void protocol_receive_command(socket_t *socket, server_t *command){
    char aux[10] = "00";
    skt_receive(socket, 1, aux);
    if (socket->status==SKT_RECEPTION_SUCCESS){
        command->command = aux[0];
        if ( aux[0] == 'P' ) {
            command->command = aux[0];
            skt_receive(socket, 1, aux);
            command->row = (uint8_t)aux[0];
            skt_receive(socket, 1, aux);
            command->col = (uint8_t)aux[0];
            skt_receive(socket, 1, aux);
            command->value = (uint8_t)aux[0];
        }
    }
}

void protocol_send_string(socket_t *self, char *command){
    char message_len[HEADER_LEN];
    int len = 0;
    
    len = strlen(command);
    if (len < 10){
        snprintf(message_len, sizeof(message_len), "00%d", len);
    } else if (len <100 && len>=10) {
        snprintf(message_len, sizeof(message_len), "%0d", len);
    } else {
        snprintf(message_len, sizeof(message_len), "%d", len);
    }
    skt_send(self, HEADER_LEN-1, message_len);
    skt_send(self, len, command);
}

void protocol_receive_string(socket_t *self, char *received){
    int len = 0;
    char header[HEADER_LEN];

    //primero recibe un header con cuantos bytes voy a recibir
    memset(header, 0, HEADER_LEN);
    skt_receive(self, HEADER_LEN-1, header);

    //esta es la cantidad de bytes de mi string a recibir
    len = atoi(header);
    if (len == 0) {
        self->status = SKT_RECEPTION_FAILURE;
    } else {
        //ahora recibo lo necesario
        skt_receive(self, len, received);
        received[len-1] = '\0';
        printf("%s\n", received);
    }
}
