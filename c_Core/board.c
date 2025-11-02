#include "board.h"
#include <stdio.h>

bool on_board(int square) {
    return !(square & 0x88);
}

void init_board(Board *board) {
    // Clear all squares
    for (int i = 0; i < BOARD_SIZE; i++) {
        board->squares[i].type = EMPTY;
        board->squares[i].color = NO_COLOR;
    }

    // Place pawns
    for (int i = 0; i < 8; i++) {
        board->squares[0x10 + i].type = PAWN;
        board->squares[0x10 + i].color = WHITE;

        board->squares[0x60 + i].type = PAWN;
        board->squares[0x60 + i].color = BLACK;
    }

    // Place other pieces (rooks, knights, bishops, queen, king)
    PieceType back_rank[8] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };

    for (int i = 0; i < 8; i++) {
        board->squares[i].type = back_rank[i];
        board->squares[i].color = WHITE;

        board->squares[0x70 + i].type = back_rank[i];
        board->squares[0x70 + i].color = BLACK;
    }

    board->current_turn = WHITE;
}

void print_board(Board *board) {
    printf("\n");
    for (int rank = 7; rank >= 0; rank--) {
        printf("%d  ", rank + 1);
        for (int file = 0; file < 8; file++) {
            int sq = (rank << 4) + file;
            Piece p = board->squares[sq];
            char c = '.';

            if (p.type != EMPTY) {
                char piece_chars[] = " PNBRQK";
                c = piece_chars[p.type];
                if (p.color == BLACK)
                    c += 32; // lowercase for black pieces
            }
            printf("%c ", c);
        }
        printf("\n");
    }
    printf("\n   a b c d e f g h\n");
}
