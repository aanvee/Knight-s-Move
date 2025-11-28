# python_GUI/main.py
import os
import sys
import pygame
from ctypes import *

# -------------------- CONFIG --------------------
dll_path = os.path.join(os.path.dirname(__file__), "chess.dll")

# -------------------- LOAD DLL -------------------
if not os.path.exists(dll_path):
    print("ERROR: chess.dll not found at:", dll_path)
    sys.exit(1)

try:
    chess_lib = CDLL(dll_path)
except OSError as e:
    print("ERROR: Failed to load chess.dll")
    print("Details:", e)
    sys.exit(1)

# -------------------- C SIGNATURES ----------------
BoardPtr = c_void_p

chess_lib.create_board.restype = BoardPtr
chess_lib.free_board.argtypes = [BoardPtr]
chess_lib.display_board.argtypes = [BoardPtr]

chess_lib.make_move.argtypes = [BoardPtr, c_int, c_int]
chess_lib.make_move.restype = c_int

chess_lib.get_board_state.argtypes = [BoardPtr, POINTER(c_char)]

chess_lib.is_check.argtypes = [BoardPtr, c_int]
chess_lib.is_check.restype = c_int

chess_lib.is_checkmate.argtypes = [BoardPtr, c_int]
chess_lib.is_checkmate.restype = c_int

chess_lib.is_stalemate.argtypes = [BoardPtr, c_int]
chess_lib.is_stalemate.restype = c_int

chess_lib.get_turn.argtypes = [BoardPtr]
chess_lib.get_turn.restype = c_int

chess_lib.clone_board.argtypes = [BoardPtr]
chess_lib.clone_board.restype = BoardPtr

chess_lib.free_board_clone.argtypes = [BoardPtr]

# -------------------- PIECE MAPPING / ASSETS ----------------
_piece_map = {
    'K': 'wK', 'Q': 'wQ', 'R': 'wR', 'B': 'wB', 'N': 'wN', 'P': 'wP',
    'k': 'bK', 'q': 'bQ', 'r': 'bR', 'b': 'bB', 'n': 'bN', 'p': 'bP',
    '.': None
}

pygame.init()
W, H = 640, 640
SCREEN = pygame.display.set_mode((W, H))
pygame.display.set_caption("Knight's Move — C Engine + Pygame GUI")
SQUARE = W // 8
FPS = 60

LIGHT = (240, 217, 181)
DARK = (181, 136, 99)
GREEN = (0, 200, 0)
RED = (255, 0, 0)
YELLOW = (255, 235, 59)
SELECT_COLOR = (255, 255, 100)
TEXT_COLOR = (10, 10, 10)

ASSETS = os.path.join(os.path.dirname(__file__), "assets")
PIECES = {}
for c in "wb":
    for p in "KQBNRP":
        img_path = os.path.join(ASSETS, f"{c}{p}.png")
        if os.path.exists(img_path):
            img = pygame.image.load(img_path).convert_alpha()
            PIECES[f"{c}{p}"] = pygame.transform.smoothscale(img, (SQUARE, SQUARE))

# -------------------- Convert C-board 64 char string to Python matrix ----------------
def c_board_to_python(board_ptr):
    buf = (c_char * 65)()
    chess_lib.get_board_state(board_ptr, buf)
    s = buf.raw[:64].decode("ascii")
    return [[_piece_map[s[r * 8 + f]] for f in range(8)] for r in range(8)]

# -------------------- Helpers -------------------
def on_board(sq): return (sq & 0x88) == 0

def get_legal_moves(board_ptr, from_sq):
    moves = []
    temp = chess_lib.clone_board(board_ptr)
    for to_sq in range(128):
        if not on_board(to_sq):
            continue
        if chess_lib.make_move(temp, from_sq, to_sq):
            moves.append(to_sq)
            chess_lib.free_board_clone(temp)
            temp = chess_lib.clone_board(board_ptr)
    chess_lib.free_board_clone(temp)
    return moves

def rc_to_board_index(r, f): return (r << 4) | f
def board_index_to_rc(sq): return (sq >> 4), (sq & 7)

# -------------------- Drawing -------------------
def draw_board(selected=None, last_move=None):
    for r in range(8):
        for f in range(8):
            color = LIGHT if ((r + f) % 2 == 0) else DARK
            pygame.draw.rect(SCREEN, color, (f * SQUARE, r * SQUARE, SQUARE, SQUARE))
    if last_move:
        fs, ts = last_move
        fr, ff = board_index_to_rc(fs)
        tr, tf = board_index_to_rc(ts)
        pygame.draw.rect(SCREEN, YELLOW, (ff * SQUARE, fr * SQUARE, SQUARE, SQUARE), 4)
        pygame.draw.rect(SCREEN, YELLOW, (tf * SQUARE, tr * SQUARE, SQUARE, SQUARE), 4)

def draw_pieces(py_board, dragging_piece=None, drag_pos=None):
    for r in range(8):
        for f in range(8):
            p = py_board[r][f]
            if p:
                if dragging_piece and dragging_piece["pos"] == (r, f):
                    continue
                SCREEN.blit(PIECES[p], (f * SQUARE, r * SQUARE))
    if dragging_piece:
        img = PIECES[dragging_piece["key"]]
        mx, my = drag_pos
        SCREEN.blit(img, (mx - SQUARE // 2, my - SQUARE // 2))

def draw_highlights(selected, legal, py_board):
    if not selected:
        return
    r, f = selected
    pygame.draw.rect(SCREEN, SELECT_COLOR, (f * SQUARE, r * SQUARE, SQUARE, SQUARE))
    piece = py_board[r][f]
    for sq in legal:
        tr, tf = board_index_to_rc(sq)
        center = (tf * SQUARE + SQUARE // 2, tr * SQUARE + SQUARE // 2)
        target = py_board[tr][tf]
        if target and target[0] != piece[0]:
            pygame.draw.rect(SCREEN, RED, (tf * SQUARE, tr * SQUARE, SQUARE, SQUARE), 4)
        else:
            pygame.draw.circle(SCREEN, GREEN, center, SQUARE // 6)

def draw_status_text(text, color=(0,0,0)):
    font = pygame.font.SysFont("Arial", 20, bold=True)
    SCREEN.blit(font.render(text, True, color), (5, 5))

def draw_game_over(txt):
    f1 = pygame.font.SysFont("Arial", 54, bold=True)
    f2 = pygame.font.SysFont("Arial", 38, bold=True)
    t1 = f1.render(txt, True, (200, 0, 0))
    t2 = f2.render("GAME OVER", True, (0, 0, 0))
    SCREEN.blit(t1, t1.get_rect(center=(W//2, H//2 - 20)))
    SCREEN.blit(t2, t2.get_rect(center=(W//2, H//2 + 40)))

# -------------------- Main --------------------
def main():
    clock = pygame.time.Clock()

    board_ptr = chess_lib.create_board()

    undo_stack = []
    redo_stack = []

    def save_state():
        clone = chess_lib.clone_board(board_ptr)
        undo_stack.append(clone)
        redo_stack.clear()

    def undo():
        nonlocal board_ptr
        if not undo_stack:
            return
        redo_stack.append(chess_lib.clone_board(board_ptr))
        chess_lib.free_board(board_ptr)
        board_ptr = undo_stack.pop()

    def redo():
        nonlocal board_ptr
        if not redo_stack:
            return
        undo_stack.append(chess_lib.clone_board(board_ptr))
        chess_lib.free_board(board_ptr)
        board_ptr = redo_stack.pop()

    selected = None
    legal_moves = []
    dragging = False
    dragging_piece = None
    last_move = None
    game_over = False
    end_text = None

    while True:
        clock.tick(FPS)
        py_board = c_board_to_python(board_ptr)
        turn = chess_lib.get_turn(board_ptr)
        turn_color = "w" if turn == 0 else "b"

        if not game_over:
            if chess_lib.is_checkmate(board_ptr, turn):
                end_text = "CHECKMATE!"
                game_over = True
            elif chess_lib.is_stalemate(board_ptr, turn):
                end_text = "STALEMATE!"
                game_over = True

        draw_board(selected, last_move)
        draw_pieces(py_board, dragging_piece, pygame.mouse.get_pos())
        draw_highlights(selected, legal_moves, py_board)

        if game_over:
            draw_game_over(end_text)
        else:
            status = ("White" if turn_color == "w" else "Black") + " to move"
            if chess_lib.is_check(board_ptr, turn):
                status += " — CHECK!"
            draw_status_text(status)

        pygame.display.flip()

        for ev in pygame.event.get():
            if ev.type == pygame.QUIT:
                chess_lib.free_board(board_ptr)
                pygame.quit()
                return

            if ev.type == pygame.KEYDOWN:
                if ev.key == pygame.K_u:   # UNDO
                    undo()
                if ev.key == pygame.K_r:   # REDO
                    redo()

            if game_over:
                continue

            if ev.type == pygame.MOUSEBUTTONDOWN and ev.button == 1:
                mx, my = ev.pos
                f, r = mx // SQUARE, my // SQUARE
                if not (0 <= r < 8 and 0 <= f < 8):
                    continue
                piece = py_board[r][f]
                if not piece or piece[0] != turn_color:
                    continue
                selected = (r, f)
                from_sq = rc_to_board_index(r, f)
                legal_moves = get_legal_moves(board_ptr, from_sq)
                dragging = True
                dragging_piece = {"key": piece, "pos": (r, f)}

            if ev.type == pygame.MOUSEBUTTONUP and ev.button == 1 and selected:
                mx, my = ev.pos
                f2, r2 = mx // SQUARE, my // SQUARE
                if (0 <= r2 < 8 and 0 <= f2 < 8):
                    to_sq = rc_to_board_index(r2, f2)
                    from_sq = rc_to_board_index(selected[0], selected[1])
                    if to_sq in legal_moves:
                        save_state()
                        if chess_lib.make_move(board_ptr, from_sq, to_sq):
                            last_move = (from_sq, to_sq)
                selected = None
                legal_moves = []
                dragging = False
                dragging_piece = None

if __name__ == "__main__":
    main()