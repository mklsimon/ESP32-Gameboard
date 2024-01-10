#!/usr/bin/python3
import time
import random
import tkinter as tk
from concurrent.futures import ThreadPoolExecutor

datas = [{"coeff_trous": 20, "coeff_poids": 1},
         ]

tetris_piece = [
    [[1, 1, 1, 1]],        # I-Piece (Barre)
    [[2, 2], [2, 2]],      # O-Piece (Carré)
    [[0, 3, 0], [3, 3, 3]],  # T-Piece
    [[0, 1, 1], [1, 1, 0]],  # S-Piece
    [[1, 1, 0], [0, 1, 1]],  # Z-Piece
    [[1, 0, 0], [1, 1, 1]],  # J-Piece
    [[0, 0, 1], [1, 1, 1]]   # L-Piece
]

class Piece:
    def __init__(self, shape, pos_x, pos_y):
        self.shape = shape
        self.X = pos_x
        self.Y = pos_y

class Tetris:
    def __init__(self, configIA):
        # Init display
        self.window = tk.Tk()
        self.window.title("Tetris")
        self.window.update()
        self.configIA = configIA

        # Init game
        self.brick_size = 25
        self.board = [[0] * 10 for _ in range(20)]
        self.piece = self.get_random_piece()
        self.nextPiece = self.get_random_piece()
        self.posX, self.posY = 0, 4
        self.canvas = tk.Canvas(self.window, width=len(self.board[0]) * self.brick_size,
                                height=len(self.board) * self.brick_size, bg='black')
        self.canvas.pack()
        self.sleep = 20
        self.scores = {1: 0, 2: 0, 3: 0, 4: 0, "total_score": 0,"lines":0}

        # Call the game loop
        self.game_loop()
        self.window.mainloop()

    # Ajoutez la méthode update_score
    def update_score(self, lines_cleared):
        scores = {1: 40, 2: 100, 3: 300, 4: 1200}  # Scores pour 1, 2, 3 et 4 lignes
        if lines_cleared in scores:
            self.scores["total_score"] += scores[lines_cleared]
            self.scores[lines_cleared] += 1
        self.scores["lines"] += lines_cleared

    def get_random_piece(self):
        return random.choice(tetris_piece)

    def merge_matrices(self, board, piece, offset_x, offset_y):
        merged_board = [row[:] for row in board]
        for i in range(len(piece)):
            for j in range(len(piece[0])):
                if piece[i][j] != 0:
                    merged_board[i + offset_x][j + offset_y] = piece[i][j]
        return merged_board

    def rotate_piece(self, piece):
        rotated_piece = [list(row) for row in zip(*reversed(piece))]
        return rotated_piece

    def draw(self, board, color="blue", offset_x=0, offset_y=0,calque=False):
        if not calque:
            self.canvas.delete("all")
        for i in range(len(board)):
            for j in range(len(board[0])):
                if board[i][j]:
                    self.canvas.create_rectangle(
                        (j + offset_y) * self.brick_size,
                        (i + offset_x) * self.brick_size,
                        (j + 1 + offset_y) * self.brick_size,
                        (i + 1 + offset_x) * self.brick_size,
                        fill=color, outline="grey"
                    )

    def check_collision(self, board, piece, offset_x, offset_y):
        if len(piece[0]) + offset_y > len(board[0]):
            return True
        if len(piece) + offset_x > len(board):
            return True        
        for i in range(len(piece)):
            for j in range(len(piece[0])):
                if board[i + offset_x][j + offset_y] != 0 and piece[i][j] != 0 :
                    return True
        return False

    def move_down(self):
        self.processIA()
        if not self.check_collision(self.board, self.piece, self.posX + 1, self.posY):
            self.posX += 1
        else:
            self.board = self.merge_matrices(self.board, self.piece, self.posX, self.posY)
            self.clear_lines()
            self.piece = self.nextPiece
            self.nextPiece = self.get_random_piece()
            self.posX, self.posY = 0, 4
            if self.check_collision(self.board, self.piece, self.posX, self.posY):
                print("Game Over")
                return False
        return True

    def get_final_scores(self):
        return self.scores
    
    def clear_lines(self):
        lines_to_clear = [i for i, row in enumerate(self.board) if all(row)]
        lines_cleared = len(lines_to_clear)
        self.update_score(lines_cleared)

        for i in lines_to_clear:
            del self.board[i]
            self.board.insert(0, [0] * 10)

    def game_loop(self):
        if not self.move_down():
            self.window.destroy()
            self.window.quit()
            return 
        board = self.merge_matrices(self.board, self.piece, self.posX, self.posY)
        self.draw(board)
        self.window.after(self.sleep, self.game_loop)

    def processIA(self):
        last_x = self.posX
        last_y = self.posY
        last_piece = self.piece
        best_score = -9999999
        best_piece = self.piece
        rotated_piece = self.rotate_piece(self.piece)
        for rotation in range(4):
            rotated_piece = self.rotate_piece(rotated_piece)
            for y_pos in range(len(self.board[0])):
                for x_pos in range(len(self.board)):  # Correction ici
                    if not self.check_collision(self.board, rotated_piece, x_pos, y_pos):
                        last_x, last_y , last_piece = x_pos, y_pos, rotated_piece
                    else:
                        temp_frame = self.merge_matrices(self.board, last_piece, last_x, last_y)
                        score = self.evaluate_position(temp_frame, last_x)
                        if score > best_score:
                            best_score = score
                            best_position_y = last_y
                            best_position_x = last_x
                            best_piece = last_piece
                        break
        self.posY = best_position_y

        self.piece = best_piece
        # Projection de la cible avec le meilleur score
        self.draw(best_piece, "green", best_position_x, best_position_y, True)
        self.window.update()
        time.sleep(.02)


    def print_matrix(self, matrix):
        print("*" * 30)
        for row in matrix:
            print(row)
        print("\n")

    def evaluate_position(self, current_board, x):
        score = x
        weight = 0
        holes = 0
        for i in range(len(current_board)):
            for j in range(len(current_board[i])):
                if current_board[i][j] != 0:
                    weight += i * i 
                else:
                    if i > 0 and current_board[i - 1][j] != 0:
                        holes += 1
        score += weight * self.configIA['coeff_poids']
        score -= holes * self.configIA['coeff_trous']
        return score

def launch(datas):
    num_threads = 1
    if num_threads == 1:
        results = [Tetris(item).get_final_scores() for item in datas]
    else:
        with ThreadPoolExecutor(max_workers=num_threads) as executor:
            futures = [executor.submit(Tetris(item).game_loop) for item in datas]
        results = [future.result().score for future in futures]
    print(results)


if __name__ == "__main__":
    window = tk.Tk()
    # window.attributes('-zoomed', True)
    window.attributes('-topmost', True)
    window.update()

    width, height = 2, 2

    # Create a toolbar frame at the top
    toolbar_frame = tk.Frame(window)
    toolbar_frame.grid(row=0, column=0, columnspan=width, sticky="ew")
    button = tk.Button(toolbar_frame, text="Start process ...", command=lambda: launch(datas))
    button.pack(side=tk.LEFT, padx=5)

    window.mainloop()
