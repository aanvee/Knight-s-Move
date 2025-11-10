# python_GUI/main.py
import pygame
import os
from ctypes import *

# --------------------------------------------------------------
# 1. LOAD DLL WITH FULL PATH (ONLY ONCE!)
# --------------------------------------------------------------
dll_path = os.path.join(os.path.dirname(__file__), "chess.dll")
if not os.path.exists(dll_path):
    raise FileNotFoundError(f"chess.dll not found: {dll_path}")

chess_lib = CDLL(dll_path)

BoardPtr = c_void_p

# Define function signatures (NOTE: make_move takes two ints (0x88 indices))
chess_lib.create_board.restype = BoardPtr
chess_lib.free_board.argtypes = [BoardPtr]
chess_lib.display_board.argtypes = [BoardPtr]
chess_lib.make_move.argtypes = [BoardPtr, c_int, c_int]
chess_lib.make_move.restype = c_int
chess_lib.get_board_state.argtypes = [BoardPtr, POINTER(c_char)]
chess_lib.is_check.argtypes = [BoardPtr, c_int]; chess_lib.is_check.restype = c_int
chess_lib.is_checkmate.argtypes = [BoardPtr, c_int]; chess_lib.is_checkmate.restype = c_int
chess_lib.is_stalemate.argtypes = [BoardPtr, c_int]; chess_lib.is_stalemate.restype = c_int
chess_lib.get_turn.argtypes = [BoardPtr]; chess_lib.get_turn.restype = c_int
chess_lib.clone_board.argtypes = [BoardPtr]
chess_lib.clone_board.restype = BoardPtr
chess_lib.free_board_clone.argtypes = [BoardPtr]

# --------------------------------------------------------------
# 2. Board conversion (get_board_state returns 64 chars linear)
# --------------------------------------------------------------
_piece_map = {
    'K': 'wK', 'Q': 'wQ', 'R': 'wR', 'B': 'wB', 'N': 'wN', 'P': 'wP',
    'k': 'bK', 'q': 'bQ', 'r': 'bR', 'b': 'bB', 'n': 'bN', 'p': 'bP',
    '.': None
}

def c_board_to_python(board_ptr):
    buf = (c_char * 65)()
    chess_lib.get_board_state(board_ptr, buf)
    s = buf.raw[:64].decode('ascii')
    return [[_piece_map[s[r*8 + f]] for f in range(8)] for r in range(8)]

# --------------------------------------------------------------
# 3. Pygame init
# --------------------------------------------------------------
pygame.init()
W, H = 480, 480
SCREEN = pygame.display.set_mode((W, H))
pygame.display.set_caption("Chess – C Engine + Python GUI")
SQUARE = W // 8
LIGHT, DARK = (240, 217, 181), (181, 136, 99)

ASSETS = os.path.join(os.path.dirname(__file__), "assets")
PIECES = {}
for c in "wb":
    for p in "KQBNRP":
        path = os.path.join(ASSETS, f"{c}{p}.png")
        if os.path.exists(path):
            img = pygame.image.load(path).convert_alpha()
            PIECES[f"{c}{p}"] = pygame.transform.smoothscale(img, (SQUARE, SQUARE))

# --------------------------------------------------------------
# 4. 0x88 helper
# --------------------------------------------------------------
def on_board(sq): return (sq & 0x88) == 0

# --------------------------------------------------------------
# 5. Get legal moves (uses 0x88 indexing; uses chess_lib.make_move to test)
# --------------------------------------------------------------
def get_legal_moves(board_ptr, from_sq):
    moves = []
    temp_ptr = chess_lib.clone_board(board_ptr)
    for to_sq in range(128):
        if not on_board(to_sq):
            continue
        # Try move on a *temporary* board
        if chess_lib.make_move(temp_ptr, from_sq, to_sq):
            moves.append(to_sq)
            # reset clone each time (to avoid accumulating changes)
            chess_lib.free_board_clone(temp_ptr)
            temp_ptr = chess_lib.clone_board(board_ptr)
    chess_lib.free_board_clone(temp_ptr)
    return moves

# --------------------------------------------------------------
# 6. Drawing
# --------------------------------------------------------------
def draw_board(selected=None, legal=None):
    for r in range(8):
        for f in range(8):
            color = LIGHT if (r + f) % 2 == 0 else DARK
            if selected == (r, f):
                color = (255, 255, 100)
            pygame.draw.rect(SCREEN, color, (f*SQUARE, r*SQUARE, SQUARE, SQUARE))
    if legal:
        for to_sq in legal:
            tr, tf = to_sq >> 4, to_sq & 7
            cx = tf * SQUARE + SQUARE // 2
            cy = tr * SQUARE + SQUARE // 2
            pygame.draw.circle(SCREEN, (0, 200, 0, 180), (cx, cy), SQUARE // 6)

def draw_pieces(board):
    for r in range(8):
        for f in range(8):
            p = board[r][f]
            if p:
                SCREEN.blit(PIECES[p], (f*SQUARE, r*SQUARE))

# --------------------------------------------------------------
# 7. Main loop
# --------------------------------------------------------------
def main():
    board_ptr = chess_lib.create_board()
    # show initial board in console (for debugging)
    chess_lib.display_board(board_ptr)

    selected = None
    legal_moves = []
    dragging = False

    while True:
        py_board = c_board_to_python(board_ptr)
        turn = chess_lib.get_turn(board_ptr)
        # Debug: print turn numeric occasionally (comment out if noisy)
        # print("DEBUG: get_turn ->", turn)
        txt = f"{'White' if turn == 1 else 'Black'}'s turn"
        if chess_lib.is_check(board_ptr, turn):
            txt += " (CHECK!)"
        if chess_lib.is_checkmate(board_ptr, turn):
            txt = "CHECKMATE!"
        if chess_lib.is_stalemate(board_ptr, turn):
            txt = "STALEMATE!"

        draw_board(selected, legal_moves if selected else None)
        draw_pieces(py_board)
        font = pygame.font.SysFont('Arial', 24)
        SCREEN.blit(font.render(txt, True, (200, 0, 0)), (10, 10))

        if dragging and selected:
            mx, my = pygame.mouse.get_pos()
            piece = py_board[selected[0]][selected[1]]
            if piece:
                SCREEN.blit(PIECES[piece], (mx - SQUARE//2, my - SQUARE//2))

        pygame.display.flip()

        for ev in pygame.event.get():
            if ev.type == pygame.QUIT:
                chess_lib.free_board(board_ptr)
                pygame.quit()
                return

            if ev.type == pygame.MOUSEBUTTONDOWN and ev.button == 1:
                mx, my = ev.pos
                f, r = mx // SQUARE, my // SQUARE
                if not (0 <= f < 8 and 0 <= r < 8):
                    continue
                piece = py_board[r][f]
                # only allow selecting piece of the side to move
                if piece:
                    if turn == 1 and piece[0] != 'w':  # 1 = WHITE’s turn
                        continue
                    if turn == 2 and piece[0] != 'b':  # 2 = BLACK’s turn
                        continue
                    selected = (r, f)
                    from_sq = (r << 4) | f
                    legal_moves = get_legal_moves(board_ptr, from_sq)
                    dragging = True


            if ev.type == pygame.MOUSEBUTTONUP and ev.button == 1 and selected:
                mx, my = ev.pos
                to_f, to_r = mx // SQUARE, my // SQUARE
                if 0 <= to_f < 8 and 0 <= to_r < 8:
                    to_sq = (to_r << 4) | to_f   # 0x88 index
                    from_sq = (selected[0] << 4) | selected[1]
                    print(f"Attempt move (0x88): {hex(from_sq)} -> {hex(to_sq)}", flush=True)
                    res = chess_lib.make_move(board_ptr, from_sq, to_sq)
                    print("make_move returned", res, flush=True)
                    # print C-side board to console so we can confirm engine actually changed state
                    chess_lib.display_board(board_ptr)
                selected = None
                legal_moves = []
                dragging = False

if __name__ == "__main__":
    main()
