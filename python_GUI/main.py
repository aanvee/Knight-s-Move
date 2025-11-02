import pygame
from ctypes import *

# Load shared C library (adjust path & name accordingly)
chess_lib = CDLL("../c_core/libchess.so")

# Define C function signatures
chess_lib.create_board.restype = c_void_p
chess_lib.free_board.argtypes = [c_void_p]
chess_lib.display_board.argtypes = [c_void_p]
chess_lib.make_move.argtypes = [c_void_p, c_int, c_int]
chess_lib.make_move.restype = c_int

# Initialize pygame
pygame.init()
WIDTH, HEIGHT = 480, 480
SCREEN = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Chess GUI")

SQUARE_SIZE = WIDTH // 8
LIGHT = (240, 217, 181)
DARK = (181, 136, 99)

def draw_board():
    for rank in range(8):
        for file in range(8):
            color = LIGHT if (rank + file) % 2 == 0 else DARK
            rect = pygame.Rect(file * SQUARE_SIZE, rank * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE)
            pygame.draw.rect(SCREEN, color, rect)
def test_moves(board_ptr):
    # Helper to convert algebraic (like 'e2') to 0x88
    def algebraic_to_0x88(square):
        files = 'abcdefgh'
        file = files.index(square[0])
        rank = int(square[1]) - 1
        return (rank << 4) + file

    from_sq = algebraic_to_0x88('e2')
    to_sq = algebraic_to_0x88('e4')

    result = chess_lib.make_move(board_ptr, from_sq, to_sq)
    print("Move e2 to e4:", "Success" if result == 1 else "Failed")

    from_sq = algebraic_to_0x88('e7')
    to_sq = algebraic_to_0x88('e5')

    result = chess_lib.make_move(board_ptr, from_sq, to_sq)
    print("Move e7 to e5:", "Success" if result == 1 else "Failed")

# Call test_moves(board_ptr) after creating board in your main()

def main():
    board_ptr = chess_lib.create_board()

    running = True
    while running:
        draw_board()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        pygame.display.flip()

    chess_lib.free_board(board_ptr)
    pygame.quit()

if __name__ == "__main__":
    main()
