#!/usr/bin/python3
import time
import random
import tkinter as tk
from concurrent.futures import ThreadPoolExecutor
import copy 

datas = [{"coeff_trous": 20, "coeff_poids": 1},
         ]

tetris_piece = [
    [[1, 1, 1, 1]],        # I-Piece (Barre)
    [[1, 1], [1, 1]],      # O-Piece (Carré)
    [[0, 1, 0], [1, 1, 1]],  # T-Piece
    [[0, 1, 1], [1, 1, 0]],  # S-Piece
    [[1, 1, 0], [0, 1, 1]],  # Z-Piece
    [[1, 0, 0], [1, 1, 1]],  # J-Piece
    [[0, 0, 1], [1, 1, 1]]   # L-Piece
]
color_mapping={1:"blue",2:"green",3:"yellow"}
class TetrisAI:
    def __init__(self, configIA):
        self.shape = [[]]
        self.configIA = configIA

    def evaluate_position(self, board, current_piece):
        best_score = 0
        piece = copy.deepcopy(current_piece)  # Create a copy to avoid modifying the original piece

        for rotation in range(4):
            piece.rotate_piece()
            for y_pos in range(board.width):
                for x_pos in range(board.height):                
                    piece.set_position(x_pos, y_pos)
                    if not board.collision(piece, x_pos, y_pos):                        
                        last_piece = copy.deepcopy(piece)  # Create a copy
                    else:
                        score = self.get_score(board, last_piece)
                        if score > best_score:
                            best_score = score
                            best_piece = copy.deepcopy(last_piece)  # Update the best piece
                        break

        self.shape = best_piece.shape
        self.X = best_piece.X
        self.Y = best_piece.Y

        return self.Y, copy.deepcopy(best_piece).shape

    def get_score(self, board, piece):
        tmpBoard = board.merge_piece(piece)
        # print_matrix(tmpBoard.shape)
        score = piece.X
        # print(score)
        # weight = 0
        # holes = 0
        # for i in range(len(tmpBoard.shape)):
        #     for j in range(len(tmpBoard.shape[i])):
        #         if tmpBoard.shape[i][j] != 0:
        #             weight += i * i 
        #         else:
        #             if i > 0 and tmpBoard.shape[i - 1][j] != 0:
        #                 holes += 1
        # score += weight * self.configIA['coeff_poids']
        # score -= holes * self.configIA['coeff_trous']
        return score
    
class TetrisPiece:
    def __init__(self, pos_x=0, pos_y=4):
        self.shape = self.get_random_piece()
        self.width, self.height = self.size_it()
        self.X = pos_x
        self.Y = pos_y

    def set_position(self,X,Y):
        self.X, self.Y = X, Y

    def get_random_piece(self):
        return random.choice(tetris_piece)
    
    def rotate_piece(self):
        self.shape = [list(row) for row in zip(*reversed(self.shape))]
        self.width, self.height = self.size_it()
    
    def size_it(self):
        return len(self.shape),len(self.shape[0])
    
class TetrisBoard:
    def __init__(self, height=20, width=10):
        self.width = width
        self.height = height
        self.shape = [[0] * width for _ in range(height)]

    def merge_piece(self, piece, value=None):
        merged = TetrisBoard(self.height, self.width)
        merged_board = [row[:] for row in self.shape]
        for i in range(len(piece.shape)):
            for j in range(len(piece.shape[0])):
                if piece.shape[i][j] != 0:
                    merged_board[i + piece.X][j + piece.Y] = value or piece.shape[i][j]
        merged.shape = merged_board
        return merged
    
    def collision(self, piece, offset_x, offset_y):        
        if len(piece.shape[0]) + offset_y > self.width:
            return True
        if len(piece.shape) + offset_x > self.height:
            return True        
        for i in range(len(piece.shape)):
            for j in range(len(piece.shape[0])):
                if self.shape[i + offset_x][j + offset_y] != 0 and piece.shape[i][j] != 0 :
                    return True
        return False

class Tetris:
    def __init__(self, configIA, height=20, width=10):
        # Init display
        self.window = tk.Tk()
        self.window.title("Tetris")

        # Config display
        self.brick_size = 25
        self.canvas = tk.Canvas(self.window, width=width * self.brick_size, height=height * self.brick_size, bg='black')
        self.canvas.pack()
        # Ajouter un Label pour afficher le score
        self.score_label = tk.Label(self.window, text="Score: 0")
        self.score_label.pack(side=tk.LEFT, padx=10)  # Ajuster la marge selon vos préférences

        # Ajouter un Label pour afficher le nombre de lignes
        self.lines_label = tk.Label(self.window, text="Lines: 0")
        self.lines_label.pack(side=tk.LEFT, padx=10)  # Ajuster la marge selon vos préférences


        # Init Game
        self.board = TetrisBoard(height, width)
        self.piece = TetrisPiece()
        self.nextPiece = TetrisPiece()
        self.ai = TetrisAI(configIA)
        self.sleep = 200
        self.scores = {1: 0, 2: 0, 3: 0, 4: 0, "total_score": 0,"lines":0}

        # Call the game loop
        self.game_loop()
        self.window.update()
        self.window.mainloop()

    # Calcul du score
    def update_score(self, lines_cleared):
        scores = {1: 40, 2: 100, 3: 300, 4: 1200}
        if lines_cleared in scores:
            self.scores["total_score"] += scores[lines_cleared]
            self.scores[lines_cleared] += 1
        self.scores["lines"] += lines_cleared
        self.score_label.config(text="Score: {}".format(self.scores["total_score"]))
        self.lines_label.config(text="Lines: {}".format(self.scores["lines"]))

    # Draw matrix
    def draw(self, board):
        self.canvas.delete("all")
        for i in range(len(board)):
            for j in range(len(board[0])):
                if board[i][j]:
                    self.canvas.create_rectangle(
                        (j) * self.brick_size,
                        (i) * self.brick_size,
                        (j + 1 ) * self.brick_size,
                        (i + 1 ) * self.brick_size,
                        fill=color_mapping[board[i][j]], outline="grey"
                    )
    def move_down(self):
        self.piece.Y, self.piece.shape = self.ai.evaluate_position(self.board, self.piece)
        if not self.board.collision(self.piece, self.piece.X + 1, self.piece.Y):
            self.piece.X += 1
        else:
            self.board = self.board.merge_piece(self.piece)
            self.clear_lines()
            self.piece = self.nextPiece
            self.nextPiece = TetrisPiece()
            if self.board.collision(self.piece, self.piece.X, self.piece.Y):
                print("Game Over")
                return False
        return True
    
    def clear_lines(self):
        lines_to_clear = [i for i, row in enumerate(self.board.shape) if all(row)]
        lines_cleared = len(lines_to_clear)
        self.update_score(lines_cleared)        
        # Clear the lines in descending order to avoid index issues
        for i in sorted(lines_to_clear, reverse=True):
            del self.board.shape[i]
            self.board.shape.insert(0, [0] * self.board.width)
            
    def game_loop(self):
        if not self.move_down():
            self.window.destroy()
            self.window.quit()
            return 
        tempBoard = self.board.merge_piece(self.ai,3)
        tempBoard = tempBoard.merge_piece(self.piece,2)
        self.draw(tempBoard.shape)
        self.window.after(self.sleep, self.game_loop)

def print_matrix(matrix):
    print("*" * 30)
    for id,row in enumerate(matrix):
        print('{:3}| '.format(id),row)
    print("\n")

def launch(datas):
    num_threads = 1
    if num_threads == 1:
        results = [Tetris(item).scores for item in datas]
    else:
        with ThreadPoolExecutor(max_workers=num_threads) as executor:
            futures = [executor.submit(Tetris(item).game_loop) for item in datas]
        results = [future.result().scores for future in futures]
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
