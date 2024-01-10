#!/home/adm-misimon/venv/bin/python

from queue import Queue
import random
import tkinter as tk
from concurrent.futures import ThreadPoolExecutor

datas = []
for i in range(1000):
    datas.append({"data": 0})

tetris_piece = [
    [[1, 1, 1, 1]],        # I-Piece (Barre)
    [[1, 1], [1, 1]],      # O-Piece (Carré)
    [[0, 1, 0], [1, 1, 1]],  # T-Piece
    [[0, 1, 1], [1, 1, 0]],  # S-Piece
    [[1, 1, 0], [0, 1, 1]],  # Z-Piece
    [[1, 0, 0], [1, 1, 1]],  # J-Piece
    [[0, 0, 1], [1, 1, 1]]   # L-Piece
]

class Tetris:
    def __init__(self,datas):
        self.canvas = self.find_free_slot()
        self.board = [[0] * 22 for _ in range(10)]
        self.piece = self.get_random_piece()
        self.nextPiece = self.get_random_piece()
        self.posX, self.posY = 0, 4
        self.draw()

    def find_free_slot(self):
        for item in tetris_canvas:
            if not item.in_use:
                item.in_use = True
                return item
    
    def get_random_piece(self):
        return random.choice(tetris_piece)

    def merge_matrices(self, board, piece):
        for i in range(len(piece)):
            for j in range(len(piece[0])):
                board[i + self.posX][j + self.posY] = piece[i][j]

    def rotate_piece(self, piece):
        rotated_piece = [list(row) for row in zip(*reversed(piece))]
        return rotated_piece
    
    def draw(self):
        for i in range(len(self.piece)):
            for j in range(len(self.piece[0])):
                if self.piece[i][j]:
                    self.canvas.create_rectangle(
                        j * 30, i * 30, (j + 1) * 30, (i + 1) * 30, fill="blue"
                    )
    
def launch(datas):
    num_threads = 1
    if num_threads == 1:
        results = [Tetris(item) for item in datas]
    else:
        with ThreadPoolExecutor(max_workers=num_threads) as executor:
            futures = [executor.submit(Tetris, item) for item in datas]
        results = [future.result() for future in futures]

if __name__ == "__main__":
    window = tk.Tk()
    window.attributes('-zoomed', True)
    window.update()

    width, height = 2, 2
    tetris_canvas = []

    # Create a toolbar frame at the top
    toolbar_frame = tk.Frame(window)
    toolbar_frame.grid(row=0, column=0, columnspan=width, sticky="ew")
    button = tk.Button(toolbar_frame, text="Cliquez-moi", command=lambda: launch(datas))
    button.pack(side=tk.LEFT, padx=5)

    for i in range(height):
        for j in range(width):
            canvas = tk.Canvas(window, width=window.winfo_width() // width - 2, height=window.winfo_height() // height - 2, borderwidth=1, relief="solid" )
            canvas.grid(row=i + 1, column=j, padx=1, pady=0)  # Adjust row index to leave space for the toolbar
            canvas.in_use = False
            tetris_canvas.append(canvas)

    window.mainloop()
