#include "move.h"
#include "status.h"
#include <stdlib.h>
#include <stdio.h>

// --- Helpers ---
static bool is_clear_path(Board *board, int from, int to, int step) {
    int sq = from + step;
    while (sq != to) {
        if (!on_board(sq)) return false;
        if (board->squares[sq].type != EMPTY)
            return false;
        sq += step;
    }
    return true;
}

static int abs_diff(int a) { return a < 0 ? -a : a; }

// --- Movement Patterns ---
static bool basic_move_ok(Board *board, int from, int to) {
    Piece piece = board->squares[from];
    Piece target = board->squares[to];
    if (piece.type == EMPTY || piece.color == NO_COLOR)
        return false;
    if (target.color == piece.color)
        return false;

    int diff = to - from;
    int rank_from = from >> 4, rank_to = to >> 4;
    int file_from = from & 7, file_to = to & 7;
    int rank_diff = rank_to - rank_from;
    int file_diff = file_to - file_from;

    switch (piece.type) {
        case PAWN: {
            int forward = (piece.color == WHITE) ? 1 : -1;
            int start_rank = (piece.color == WHITE) ? 1 : 6;
            int promotion_rank = (piece.color == WHITE) ? 7 : 0;

            // Move forward
            if (file_diff == 0) {
                if (rank_diff == forward && target.type == EMPTY)
                    return true;
                if (rank_from == start_rank && rank_diff == 2 * forward &&
                    board->squares[from + (forward << 4)].type == EMPTY &&
                    target.type == EMPTY)
                    return true;
            }

            // Capture
            if (abs_diff(file_diff) == 1 && rank_diff == forward &&
                target.type != EMPTY && target.color != piece.color)
                return true;

            return false;
        }

        case KNIGHT:
            return (abs_diff(rank_diff) == 2 && abs_diff(file_diff) == 1) ||
                   (abs_diff(rank_diff) == 1 && abs_diff(file_diff) == 2);

        case BISHOP:
            if (abs_diff(rank_diff) == abs_diff(file_diff))
                return is_clear_path(board, from, to,
                                     ((rank_diff > 0) ? 0x10 : -0x10) +
                                     ((file_diff > 0) ? 1 : -1));
            return false;

        case ROOK:
            if (rank_diff == 0)
                return is_clear_path(board, from, to, (file_diff > 0) ? 1 : -1);
            if (file_diff == 0)
                return is_clear_path(board, from, to, (rank_diff > 0) ? 0x10 : -0x10);
            return false;

        case QUEEN:
            if (rank_diff == 0)
                return is_clear_path(board, from, to, (file_diff > 0) ? 1 : -1);
            if (file_diff == 0)
                return is_clear_path(board, from, to, (rank_diff > 0) ? 0x10 : -0x10);
            if (abs_diff(rank_diff) == abs_diff(file_diff))
                return is_clear_path(board, from, to,
                                     ((rank_diff > 0) ? 0x10 : -0x10) +
                                     ((file_diff > 0) ? 1 : -1));
            return false;

        case KING:
            if (abs_diff(rank_diff) <= 1 && abs_diff(file_diff) <= 1)
                return true;
            // Castling attempt
            if (rank_diff == 0 && abs_diff(file_diff) == 2)
                return true; // Will verify separately
            return false;

        default:
            return false;
    }
}

// --- Core Move Validation (with King Safety) ---
bool is_valid_move(Board *board, int from, int to) {
    if (!on_board(from) || !on_board(to)) return false;

    Piece moving = board->squares[from];
    if (moving.type == EMPTY || moving.color == NO_COLOR) return false;

    if (!basic_move_ok(board, from, to)) return false;

    // --- Handle Castling ---
    if (moving.type == KING && abs((to & 7) - (from & 7)) == 2) {
        int rank = from >> 4;
        int king_side = (to & 7) > (from & 7);
        int rook_from = rank * 16 + (king_side ? 7 : 0);
        int rook_to = rank * 16 + (king_side ? 5 : 3);
        Piece rook = board->squares[rook_from];

        if (rook.type != ROOK || rook.color != moving.color)
            return false;
        if (!is_clear_path(board, from, rook_from, king_side ? 1 : -1))
            return false;
        if (is_check(board, moving.color))
            return false;

        // Temporarily simulate castling king movement
        board->squares[to] = moving;
        board->squares[from].type = EMPTY;
        board->squares[from].color = NO_COLOR;
        bool safe = !is_check(board, moving.color);
        board->squares[from] = moving;
        board->squares[to].type = EMPTY;
        board->squares[to].color = NO_COLOR;
        return safe;
    }

    // --- Normal move: check self-check rule ---
    Piece captured = board->squares[to];
    board->squares[to] = moving;
    board->squares[from].type = EMPTY;
    board->squares[from].color = NO_COLOR;

    int in_check = is_check(board, moving.color);

    // Undo move
    board->squares[from] = moving;
    board->squares[to] = captured;

    return !in_check;
}

// --- Execute move with promotion/castling ---
int make_move(Board *board, int from, int to) {
    if (!is_valid_move(board, from, to))
        return 0;

    Piece moving = board->squares[from];
    int rank_to = to >> 4;

    // Castling execution
    if (moving.type == KING && abs((to & 7) - (from & 7)) == 2) {
        int rank = from >> 4;
        int king_side = (to & 7) > (from & 7);
        int rook_from = rank * 16 + (king_side ? 7 : 0);
        int rook_to = rank * 16 + (king_side ? 5 : 3);

        board->squares[to] = moving;
        board->squares[from].type = EMPTY;
        board->squares[from].color = NO_COLOR;

        board->squares[rook_to] = board->squares[rook_from];
        board->squares[rook_from].type = EMPTY;
        board->squares[rook_from].color = NO_COLOR;

        printf("Castling performed!\n");
    } else {
        // Normal move
        board->squares[to] = moving;
        board->squares[from].type = EMPTY;
        board->squares[from].color = NO_COLOR;
    }

    // Pawn promotion
    if (moving.type == PAWN &&
        ((moving.color == WHITE && rank_to == 7) ||
         (moving.color == BLACK && rank_to == 0))) {
        board->squares[to].type = QUEEN; // Auto-promote to Queen
        printf("Pawn promoted to Queen!\n");
    }

    board->current_turn = (board->current_turn == WHITE) ? BLACK : WHITE;
    return 1;
}
