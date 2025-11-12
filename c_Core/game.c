#include "game.h"
#include "board.h"
#include <string.h>
 // for memcpy

void init_game_stack(GameStack *stack, Board *initial_board) {
    stack->current_index = 0;
    stack->top_index = 0;
    memcpy(&stack->history[0].board_state, initial_board, sizeof(Board));
}

void push_state(GameStack *stack, Board *board) {
    // If we make a new move after undo, drop any “future” states
    if (stack->current_index < stack->top_index) {
        stack->top_index = stack->current_index;
    }

    if (stack->top_index + 1 < MAX_HISTORY) {
        stack->top_index++;
        stack->current_index = stack->top_index;
        memcpy(&stack->history[stack->top_index].board_state, board, sizeof(Board));
    }
}

int undo_move(GameStack *stack, Board *board) {
    if (stack->current_index == 0) {
        return 0;  // no more undo
    }
    stack->current_index--;
    memcpy(board, &stack->history[stack->current_index].board_state, sizeof(Board));
    return 1;
}

int redo_move(GameStack *stack, Board *board) {
    if (stack->current_index >= stack->top_index) {
        return 0;  // nothing to redo
    }
    stack->current_index++;
    memcpy(board, &stack->history[stack->current_index].board_state, sizeof(Board));
    return 1;
}