#include "board.h"
#include "move.h"
#include <stdbool.h>
#include"interface.h"

// Helper: find the square index of the king for a color
int find_king(Board *board, int color) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (!on_board(i)) continue;
        Piece p = board->squares[i];
        if (p.type == KING && p.color == color)
            return i;
    }
    return -1;
}

// Check if a given color's king is under attack
int is_check(Board *board, int color) {
    int king_sq = find_king(board, color);
    if (king_sq == -1) return 0; // King missing (shouldn't happen)

    for (int i = 0; i < BOARD_SIZE; i++) {
        if (!on_board(i)) continue;
        Piece p = board->squares[i];
        if (p.color != color && p.color != NO_COLOR) {
            if (is_valid_move(board, i, king_sq)) {
                return 1; // Opponent can attack king
            }
        }
    }
    return 0;
}

// Checkmate: king is in check and no valid move can remove the check
int is_checkmate(Board *board, int color) {
    if (!is_check(board, color))
        return 0; // Not in check → can't be checkmate

    for (int from = 0; from < BOARD_SIZE; from++) {
        if (!on_board(from)) continue;
        Piece piece = board->squares[from];
        if (piece.color != color) continue;

        for (int to = 0; to < BOARD_SIZE; to++) {
            if (!on_board(to)) continue;
            if (!is_valid_move(board, from, to)) continue;

            // Save original state
            Piece captured = board->squares[to];
            board->squares[to] = piece;
            board->squares[from].type = EMPTY;
            board->squares[from].color = NO_COLOR;

            int still_in_check = is_check(board, color);

            // Undo move
            board->squares[from] = piece;
            board->squares[to] = captured;

            if (!still_in_check)
                return 0; // Found a move that escapes check
        }
    }
    return 1; // No legal move to escape → checkmate
}

// Stalemate: not in check but no legal move exists
int is_stalemate(Board *board, int color) {
    if (is_check(board, color))
        return 0; // In check → not stalemate

    for (int from = 0; from < BOARD_SIZE; from++) {
        if (!on_board(from)) continue;
        Piece piece = board->squares[from];
        if (piece.color != color) continue;

        for (int to = 0; to < BOARD_SIZE; to++) {
            if (!on_board(to)) continue;
            if (!is_valid_move(board, from, to)) continue;

            // Simulate move
            Piece captured = board->squares[to];
            board->squares[to] = piece;
            board->squares[from].type = EMPTY;
            board->squares[from].color = NO_COLOR;

            int still_in_check = is_check(board, color);

            // Undo move
            board->squares[from] = piece;
            board->squares[to] = captured;

            if (!still_in_check)
                return 0; // At least one legal move available
        }
    }
    return 1; // No moves and not in check → stalemate
}
