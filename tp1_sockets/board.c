#include "protocol.h"
#include "client.h"
#include "server.h"
#include "socket.h"
#include "board.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

void board_read(board_t *self){
    FILE * pboard;
    char c;
    int i = 0;

    if ( (pboard = fopen(BOARD_FILE,"r")) == NULL ){
        self->status = BOARD_FILE_ERROR;
    } else {
        while ( (c = fgetc(pboard)) != EOF ){
            self->board[i] = c;
            self->board_original[i] = c;
            i++;
        }
        self->status = BOARD_OK;
        fclose(pboard);
    }
}

char board_value_get(board_t *self, uint8_t i, uint8_t j){
    uint8_t tmp = 0;

    while (j > 1){
        tmp = tmp + 2;
        j--;
    }

    return self->board[CHARS_PER_ROW*(i-1) + tmp];
}

char board_value_get_original(board_t *self, uint8_t i, uint8_t j){
    uint8_t tmp = 0;

    while (j > 1){
        tmp = tmp + 2;
        j--;
    }

    return self->board_original[CHARS_PER_ROW*(i-1) + tmp ];
}

void board_value_set(board_t *self, uint8_t i, uint8_t j, uint8_t value){
    uint8_t tmp = 0;

    while (j > 1){
        tmp = tmp + 2;
        j--;
    }

    self->board[CHARS_PER_ROW*(i-1) + tmp ] = (char)value + 48;
}

void board_put(board_t *self, uint8_t i, uint8_t j, uint8_t value){
    if (((int)board_value_get_original(self, i, j)-48)!=0){
        self->status = BOARD_NON_MODIFIABLE_VALUE;
    } else {
        board_value_set(self, i, j, value);
    }
}

void board_verify(board_t *self){
    board_row_verify(self);
    board_col_verify(self);
    board_cell_verify(self);
}

void board_row_verify(board_t *self){
    short row = 0; //Bits para marcar digitos xxxx-xxx1-2345-6789
    int value = 0;

    for (int i=1; i<=9; i++){
        for (int j=1; j<=9; j++){
            value = (int)board_value_get(self, i, j) - 48; //paso a int
            if ( ( (row >> (9 - value)) & 1) ) {
                self->status = BOARD_INVALID;
            } else {
                if (value != 0)
                    row = row | (1 << (9-value));
            }
        }
        row = 0;
    }
}

void board_col_verify(board_t *self){
    short col = 0; //Bits para marcar digitos xxxx-xxx1-2345-6789
    int value = 0;

    for (int i=1; i<=9; i++){
        for (int j=1; j<=9; j++){
            value = (int)board_value_get(self, j, i) - 48; //paso a int
            if ( ( (col >> (9 - value)) & 1) ) {
                self->status = BOARD_INVALID;
            } else {
                if (value != 0)
                    col = col | (1 << (9-value));
            }
        }
        col = 0;
    }
}

//La extensión de esta función se debe a que debe recorrer cada celda
//lo cual resulta en un algoritmo particular.
void board_cell_verify(board_t *self){
    short cell = 0; //Bits para marcar digitos xxxx-xxx1-2345-6789
    int value = 0;

    //9 celdas
    for (int i = 1; i <= 3; i++){
        for (int j = 1; j <= 3; j++){
            //verifico una celda
            for (int k = 3*i-2; k <= 3*i; k++){
                for (int l = 3*j-2; l <= 3*j; l++){
                    value = (int)board_value_get(self, k, l) - 48; //paso a int
                    if ( ( (cell >> (9 - value)) & 1) ) {
                        self->status = BOARD_INVALID;
                    } else {
                        if (value != 0)
                            cell = cell | (1 << (9-value));
                    }
                }
            }
            cell = 0;
        }
    }
}

void board_reset(board_t *self){
        snprintf(self->board, sizeof(self->board), "%s", self->board_original);
        printf("%s", MSG_BOARD_RESTART);
        self->status = BOARD_OK;
}
