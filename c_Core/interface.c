// c_Core/interface.c
#include "interface.h"
#include "move.h"
#include "status.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

EXPORT Board* create_board(void) {
    Board* b = (Board*)malloc(sizeof(Board));
    if (b) init_board(b);
    return b;
}

EXPORT void free_board(Board* board) {
    free(board);
}

EXPORT void display_board(Board* board) {
    if (board) print_board(board);
}

/* NO make_move here — defined in move.c */

EXPORT void get_board_state(Board* board, char* out64) {
    if (!board || !out64) { if (out64) out64[0] = '\0'; return; }
    int idx = 0;
    for (int r = 0; r < 8; ++r) {
        for (int f = 0; f < 8; ++f) {
            int sq = (r << 4) + f;
            Piece p = board->squares[sq];
            char ch = '.';
            if (p.type != EMPTY) {
                const char* pcs = " PNBRQK";
                ch = pcs[p.type];
                if (p.color == BLACK) ch += 32;
            }
            out64[idx++] = ch;
        }
    }
    out64[idx] = '\0';
}

/* NO is_check, is_checkmate, is_stalemate — defined in status.c */

EXPORT int get_turn(Board* board) {
    return board ? board->current_turn : 0;
}
__declspec(dllexport) Board* clone_board(Board *b) {
    Board *copy = malloc(sizeof(Board));
    memcpy(copy, b, sizeof(Board));
    return copy;
}

__declspec(dllexport) void free_board_clone(Board *b) {
    free(b);
}


// #include <stdlib.h>
// #include "interface.h"
// #include <stdio.h>
// #include "move.h"
// #include "status.h"
// #include<string.h> 

// Board* create_board() {
//     Board* board = (Board*)malloc(sizeof(Board));
//     if (board != NULL) {
//         init_board(board);
//     }
//     return board;
// }

// void free_board(Board* board) {
//     if (board != NULL) {
//         free(board);
//     }
// }

// void display_board(Board* board) {
//     if (board != NULL) {
//         print_board(board);
//     }
// }
// void get_board_state(Board* board, char *out64) {
//     if (!board || !out64) {
//         if(out64) out64[0] = '\0';
//         return;
//     }

//     int idx = 0;
//     for (int rank = 0; rank < 8; rank++) {
//         for (int file = 0; file < 8; file++) {
//             int sq = (rank << 4) + file; // rank 0 is rank1 (a1..h1)
//             Piece p = board->squares[sq];
//             char ch = '.';
//             if (p.type != EMPTY) {
//                 // map PieceType to char: PieceType enum has 0..6 with EMPTY at 0
//                 char piece_chars[] = " PNBRQK";
//                 ch = piece_chars[p.type];
//                 if (p.color == BLACK) ch = ch + 32; // lowercase for black
//             }
//             out64[idx++] = ch;
//         }
//     }
//     out64[idx] = '\0';
// }
// /*int make_move(Board* board, int from, int to) {
//     if (board == NULL) return 0;
//     if (!on_board(from) || !on_board(to)) return 0;

//     Piece p = board->squares[from];
//     if (p.type == EMPTY) return 0;
//     if (p.color != board->current_turn) return 0;

//     // Use move validation
//     if (!is_valid_move(board, from, to)) return 0;

//     // Make move
//     board->squares[to] = p;
//     board->squares[from].type = EMPTY;
//     board->squares[from].color = NO_COLOR;
// // Pawn promotion: if pawn reaches final rank
//     int to_rank = to >> 4;
//     if (p.type == PAWN) {
//         if ((p.color == WHITE && to_rank == 7) ||
//             (p.color == BLACK && to_rank == 0)) {
//             // promote to queen
//             board->squares[to].type = QUEEN;
//         }}
//     // Switch turn
//     board->current_turn = (board->current_turn == WHITE) ? BLACK : WHITE;

//     return 1;
// }*/
