// gamestack.c
#include <stdlib.h>
#include "gamestack.h"
#include "board.h"

// Create new node
static StackNode* new_node(Board *b) {
    StackNode *n = malloc(sizeof(StackNode));
    n->board = b;
    n->prev = NULL;
    n->next = NULL;
    return n;
}

GameStack* create_stack(Board *initial_board) {
    GameStack *s = malloc(sizeof(GameStack));

    // clone initial board for stack
    Board *b = clone_board(initial_board);
    s->current = new_node(b);

    return s;
}

void free_stack(GameStack *stack) {
    StackNode *cur = stack->current;

    // move to bottom
    while (cur->prev)
        cur = cur->prev;

    // free upwards
    while (cur) {
        StackNode *next = cur->next;
        free_board(cur->board);
        free(cur);
        cur = next;
    }

    free(stack);
}

Board* get_board_stack(GameStack *stack) {
    return stack->current->board;
}

// push new snapshot on stack
int make_move_stack(GameStack *stack, int from_sq, int to_sq) {
    Board *cur = stack->current->board;

    // clone the board before trying
    Board *newb = clone_board(cur);

    if (!make_move(newb, from_sq, to_sq)) {
        free_board(newb);
        return 0;
    }

    // clear redo history
    if (stack->current->next) {
        StackNode *t = stack->current->next;
        while (t) {
            StackNode *nx = t->next;
            free_board(t->board);
            free(t);
            t = nx;
        }
        stack->current->next = NULL;
    }

    StackNode *n = new_node(newb);
    n->prev = stack->current;
    stack->current->next = n;
    stack->current = n;

    return 1;
}

int undo_stack(GameStack *stack) {
    if (!stack->current->prev)
        return 0;

    stack->current = stack->current->prev;
    return 1;
}

int redo_stack(GameStack *stack) {
    if (!stack->current->next)
        return 0;

    stack->current = stack->current->next;
    return 1;
}