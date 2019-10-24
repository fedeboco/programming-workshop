#define SUCCESS 0
#define FAILURE 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "protocol.h"
#include "client.h"
#include "server.h"
#include "socket.h"
#include "board.h"
#include "sudoku.h"

int main(int argc, char *argv[]){
  sudoku_t game;
  
  sudoku_set_mode(&game, argc, argv);
  if (game.status == SUDOKU_ARGS_OK)
    sudoku_run(&game);
  if (game.status != SUDOKU_ENDGAME)
    return FAILURE;
  return SUCCESS;
}
