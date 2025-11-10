from PIL import Image
import os

# Load your PNG sprite sheet (make sure name matches)
img = Image.open("chess_pieces.png")

# We'll take only the first two rows: black (row 0) and white (row 1)
rows, cols = 2, 6

# Piece naming order: King, Queen, Bishop, Rook, Knight, Pawn
piece_codes = ["K", "Q", "B", "R", "N", "P"]
colors = ["b", "w"]  # black, white

# Get dimensions
full_w, full_h = img.size
piece_w = full_w // 6
piece_h = full_h // 4  # since full sprite has 4 rows

# Save directly into assets (current directory)
output_dir = os.path.dirname(__file__)

for r in range(rows):
    for c in range(cols):
        left = c * piece_w
        top = r * piece_h
        right = left + piece_w
        bottom = top + piece_h
        piece = img.crop((left, top, right, bottom))

        # e.g. bK.png, wQ.png, etc.
        filename = f"{colors[r]}{piece_codes[c]}.png"
        path = os.path.join(output_dir, filename)
        piece.save(path)

print("✅ Cropping complete! Saved individual pieces into:", output_dir)
