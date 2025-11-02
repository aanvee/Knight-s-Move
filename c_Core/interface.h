#ifndef INTERFACE_H
#define INTERFACE_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialize and return pointer to a new board
Board* create_board();

// Free allocated board memory
void free_board(Board* board);

// Print the board state (for debug)
void display_board(Board* board);

// Make a move - simplified: from/to as 0x88 squares, returns 1 if move successful, 0 otherwise
int make_move(Board* board, int from, int to);

#ifdef __cplusplus
}
#endif

#endif
