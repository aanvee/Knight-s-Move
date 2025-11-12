#ifndef GAME_H
#define GAME_H

#include "board.h"

#define MAX_HISTORY 1024  // maximum moves stored for undo/redo

typedef struct {
    Board board_state;  // snapshot of the board
} GameState;

typedef struct {
    GameState history[MAX_HISTORY];
    int current_index;   // index of current board state
    int top_index;       // highest point in history (for redo)
} GameStack;

// Initialize stack and push the starting position
void init_game_stack(GameStack *stack, Board *initial_board);

// Push current board to history after a move
void push_state(GameStack *stack, Board *board);

// Undo last move (returns 1 if successful)
int undo_move(GameStack *stack, Board *board);

// Redo last undone move (returns 1 if successful)
int redo_move(GameStack *stack, Board *board);

#endif
