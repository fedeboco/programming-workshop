#ifndef BOARDH
#define BOARDH

#define BOARD_SIZE 200
#define BOARD_SENT_SIZE 723
#define CHARS_PER_ROW 18

#define BOARD_FILE "board.txt"
#define MSG_BOARD_RESTART "Tablero reiniciado."

typedef enum {
    BOARD_OK,
    BOARD_NON_MODIFIABLE_VALUE,
    BOARD_FILE_ERROR,
    BOARD_INVALID
} board_status_t;

typedef struct {
    char board[BOARD_SIZE];
    char board_original[BOARD_SIZE];
    board_status_t status;
} board_t;

void board_read(board_t *self);
void board_verify(board_t *self);
void board_row_verify(board_t *self);
void board_col_verify(board_t *self);
void board_cell_verify(board_t *self);
void board_put(board_t *self, uint8_t i, uint8_t j, uint8_t value);
void board_reset(board_t *self);
void board_send(board_t *self);
char board_value_get(board_t *self, uint8_t i, uint8_t j);
char board_value_get_original(board_t *self, uint8_t i, uint8_t j);
void board_value_set(board_t *self, uint8_t i, uint8_t j, uint8_t value);

#endif
