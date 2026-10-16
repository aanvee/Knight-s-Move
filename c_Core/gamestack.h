// gamestack.h
#ifndef GAMESTACK_H
#define GAMESTACK_H

#include "board.h"

typedef struct StackNode {
    Board *board;
    struct StackNode *prev;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *current;
} GameStack;

// create initial stack with starting board
GameStack* create_stack(Board *initial_board);

// free entire stack
void free_stack(GameStack *stack);

// get current board pointer
Board* get_board_stack(GameStack *stack);

// push new board from making a move
// returns 1 if successful, 0 if illegal
int make_move_stack(GameStack *stack, int from_sq, int to_sq);

// undo → return 1 if worked, 0 if cannot
int undo_stack(GameStack *stack);

// redo → return 1 if worked, 0 if cannot
int redo_stack(GameStack *stack);

#endif