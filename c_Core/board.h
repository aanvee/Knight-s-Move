#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#define BOARD_SIZE 128

typedef enum
{
    EMPTY = 0,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} PieceType;

typedef enum
{
    WHITE = 0,
    BLACK = 1,
    NO_COLOR = 2
} Color;

typedef struct
{
    PieceType type;
    Color color;
} Piece;

typedef struct
{
    Piece squares[BOARD_SIZE]; // 0x88 board
    Color current_turn;
} Board;

// Initialize the board to starting position
void init_board(Board *board);

// Print board to console for debugging
void print_board(Board *board);

// Check if a square is on the board (0x88)
bool on_board(int square);

#endif
