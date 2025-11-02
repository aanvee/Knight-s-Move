#ifndef STATUS_H
#define STATUS_H

#include "board.h"
#include <stdbool.h>

int is_check(Board *board, int color);
int is_checkmate(Board *board, int color);
int is_stalemate(Board *board, int color);

#endif
