// c_Core/interface.h
#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdlib.h>
#include "board.h"

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

Board* clone_board(Board* original);
void free_board_clone(Board* b);

EXPORT Board* create_board(void);
EXPORT void   free_board(Board* board);
EXPORT void   display_board(Board* board);
EXPORT int    make_move(Board* board, int from, int to);
EXPORT void   get_board_state(Board* board, char* out64);

EXPORT int    is_check(Board* board, int color);      // color = WHITE/BLACK
EXPORT int    is_checkmate(Board* board, int color);
EXPORT int    is_stalemate(Board* board, int color);
EXPORT int    get_turn(Board* board);                // returns current_turn

#ifdef __cplusplus
}
#endif

#endif
// #ifndef INTERFACE_H
// #define INTERFACE_H

// #include "board.h"

// #ifdef __cplusplus
// extern "C" {
// #endif

// // Initialize and return pointer to a new board
// Board* create_board();

// // Free allocated board memory
// void free_board(Board* board);

// // Print the board state (for debug)
// void display_board(Board* board);

// // Make a move - simplified: from/to as 0x88 squares, returns 1 if move successful, 0 otherwise
// int make_move(Board* board, int from, int to);

// void get_board_state(Board* board, char *out64); // out64 must be >=65 bytes (64 chars + NUL)
// #ifdef __cplusplus
// }
// #endif

// #endif
