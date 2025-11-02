#ifndef MOVE_H
#define MOVE_H

#include "board.h"
#include <stdbool.h>

// Check if a move from 'from' to 'to' is valid given the current board state
bool is_valid_move(Board* board, int from, int to);

#endif
