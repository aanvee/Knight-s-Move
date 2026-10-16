// // c_Core/interface.c
// #include "interface.h"
// #include "move.h"
// #include "status.h"
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include "game.h"

// static GameStack stack;

// int undo_move_gui(void) {
//     return undo_move(&stack, current_board_pointer);
// }

// int redo_move_gui(void) {
//     return redo_move(&stack, current_board_pointer);
// }

// EXPORT Board* create_board(void) {
//     Board* b = (Board*)malloc(sizeof(Board));
//     if (b) init_board(b);
//     return b;
// }

// EXPORT void free_board(Board* board) {
//     free(board);
// }

// EXPORT void display_board(Board* board) {
//     if (board) print_board(board);
// }

// /* NO make_move here — defined in move.c */

// EXPORT void get_board_state(Board* board, char* out64) {
//     if (!board || !out64) { if (out64) out64[0] = '\0'; return; }
//     int idx = 0;
//     for (int r = 0; r < 8; ++r) {
//         for (int f = 0; f < 8; ++f) {
//             int sq = (r << 4) + f;
//             Piece p = board->squares[sq];
//             char ch = '.';
//             if (p.type != EMPTY) {
//                 const char* pcs = " PNBRQK";
//                 ch = pcs[p.type];
//                 if (p.color == BLACK) ch += 32;
//             }
//             out64[idx++] = ch;
//         }
//     }
//     out64[idx] = '\0';
// }

// /* NO is_check, is_checkmate, is_stalemate — defined in status.c */

// EXPORT int get_turn(Board* board) {
//     return board ? board->current_turn : 0;
// }
// __declspec(dllexport) Board* clone_board(Board *b) {
//     Board *copy = malloc(sizeof(Board));
//     memcpy(copy, b, sizeof(Board));
//     return copy;
// }

// __declspec(dllexport) void free_board_clone(Board *b) {
//     free(b);
// }

// int undo_move_gui(void);
// int redo_move_gui(void);

// c_Core/interface.c

#include "interface.h"
#include "move.h"
#include "status.h"
#include "board.h"
#include "game.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static GameStack stack;
static Board *current_board_pointer = NULL;

/* ================== GUI WRAPPERS ================== */

__declspec(dllexport) int undo_move_gui(void) {
    if (!current_board_pointer) return 0;
    return undo_move(&stack, current_board_pointer);
}

__declspec(dllexport) int redo_move_gui(void) {
    if (!current_board_pointer) return 0;
    return redo_move(&stack, current_board_pointer);
}

/* ================== BOARD LIFECYCLE ================== */

__declspec(dllexport) Board* create_board(void) {
    Board* b = (Board*)malloc(sizeof(Board));
    if (b) {
        init_board(b);
        init_game_stack(&stack, b);   // ✅ VERY IMPORTANT
        current_board_pointer = b;   // ✅ VERY IMPORTANT
    }
    return b;
}

__declspec(dllexport) void free_board(Board* board) {
    if (board) {
        free(board);
        if (board == current_board_pointer)
            current_board_pointer = NULL;
    }
}

__declspec(dllexport) void display_board(Board* board) {
    if (board)
        print_board(board);
}

/* ================== BOARD EXPORT ================== */

__declspec(dllexport) void get_board_state(Board* board, char* out64) {
    if (!board || !out64) {
        if (out64) out64[0] = '\0';
        return;
    }

    int idx = 0;
    for (int r = 0; r < 8; ++r) {
        for (int f = 0; f < 8; ++f) {
            int sq = (r << 4) + f;
            Piece p = board->squares[sq];
            char ch = '.';

            if (p.type != EMPTY) {
                const char* pcs = " PNBRQK";
                ch = pcs[p.type];
                if (p.color == BLACK)
                    ch += 32;
            }
            out64[idx++] = ch;
        }
    }
    out64[idx] = '\0';
}

__declspec(dllexport) int get_turn(Board* board) {
    return board ? board->current_turn : 0;
}

/* ================== BOARD CLONE ================== */

__declspec(dllexport) Board* clone_board(Board *b) {
    if (!b) return NULL;
    Board *copy = (Board*)malloc(sizeof(Board));
    memcpy(copy, b, sizeof(Board));
    return copy;
}

__declspec(dllexport) void free_board_clone(Board *b) {
    free(b);
}
