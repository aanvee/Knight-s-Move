#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "interface.h"
#include "status.h"

#define WHITE 0
#define BLACK 1

// Converts square like "e2" to 0x14 format
int parse_square(const char *input)
{
    if (strlen(input) != 2)
        return -1;
    char file = tolower(input[0]);
    char rank = input[1];

    if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
        return -1;

    int file_index = file - 'a';
    int rank_index = rank - '1';

    return (rank_index << 4) | file_index;
}

int main()
{
    Board *board = create_board();
    char from_str[6], to_str[6];
    int from, to;
    int move_count = 0;

    printf("Welcome to Two-Player Chess!\n");
    display_board(board);

    while (1)
    {
        int current_player = (board->current_turn == WHITE) ? WHITE : BLACK;

        printf("\n%s to move. Enter move (e.g., e2 e4) or 'quit': ",
               current_player == WHITE ? "White" : "Black");

        if (scanf("%5s", from_str) != 1)
        {
            printf("Input error.\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (strcmp(from_str, "quit") == 0 || strcmp(from_str, "QUIT") == 0)
        {
            printf("Game terminated by user.\n");
            break;
        }

        if (scanf("%5s", to_str) != 1)
        {
            printf("Invalid input. Please enter both from and to squares.\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        from = parse_square(from_str);
        to = parse_square(to_str);

        if (from == -1 || to == -1)
        {
            printf("Invalid square(s). Use format like 'e2 e4'.\n");
            continue;
        }

        if (make_move(board, from, to))
        {
            printf("Move %s to %s successful!\n", from_str, to_str);
            move_count++;
            display_board(board);

            // After make_move, the turn switches internally.
            // So the opponent is now the player who just moved.
            int opponent = (board->current_turn == WHITE) ? BLACK : WHITE;

            if (is_checkmate(board, board->current_turn))
            {
                printf("Checkmate! %s wins!\n",
                       opponent == WHITE ? "White" : "Black");
                display_board(board);
                break;
            }
            else if (is_stalemate(board, board->current_turn))
            {
                printf("Stalemate! The game is a draw.\n");
                display_board(board);
                break;
            }
            else if (is_check(board, board->current_turn))
            {
                printf("%s is in check!\n",
                       board->current_turn == WHITE ? "White" : "Black");
            }
        }
        else
        {
            printf("Invalid move from %s to %s. Try again.\n", from_str, to_str);
        }
    }

    free_board(board);
    return 0;
}
