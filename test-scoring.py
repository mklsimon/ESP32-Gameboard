#!/usr/bin/python3
import time
import random
import tkinter as tk
from concurrent.futures import ThreadPoolExecutor

datas = [{"coeff_lines": 10, "coeff_trous": 10, "coeff_poids": 10}]

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
    def __init__(self, configIA):
        # Init display
        self.window = tk.Tk()
        self.window.title("Tetris")
        self.window.update()

        # Init game
        self.brick_size = 50
        self.board = [[0] * 10 for _ in range(20)]
        self.piece = self.get_random_piece()
        self.nextPiece = self.get_random_piece()
        self.posX, self.posY = 0, 4
        self.canvas = tk.Canvas(self.window, width=len(self.board[0])*self.brick_size, height=len(self.board)*self.brick_size, bg='black')
        self.canvas.pack()
        self.sleep = 500

        # Call the game loop
        self.game_loop()
        self.window.mainloop()

    def get_random_piece(self):
        return random.choice(tetris_piece)

    def merge_matrices(self, board, piece, offset_x, offset_y):
        print("Merge: board")
        self.print_matrix(board)         
        print("Merge: piece")
        self.print_matrix(piece)   
        merged_board = [row[:] for row in board]
        print("Merge: merged_board")
        self.print_matrix(merged_board)   
        for i in range(len(piece)):
            for j in range(len(piece[0])):
                if (
                    0 <= i + offset_x < len(board)
                    and 0 <= j + offset_y < len(board[0])
                ):
                    merged_board[i + offset_x][j + offset_y] += piece[i][j]
        return merged_board

    def rotate_piece(self, piece):
        rotated_piece = [list(row) for row in zip(*reversed(piece))]
        return rotated_piece

    def draw(self, board, color="blue"):
        self.canvas.delete("all")
        for i in range(len(board)):
            for j in range(len(board[0])):
                if board[i][j]:
                    self.canvas.create_rectangle(j * self.brick_size, i * self.brick_size, (j + 1) * self.brick_size, (i + 1) * self.brick_size, fill=color, outline="grey")

    def check_collision(self, board, piece, offset_x, offset_y):
        for i in range(len(piece)):
            for j in range(len(piece[0])):
                if (
                    piece[i][j] != 0
                    and (
                        i + offset_x >= len(board)
                        or j + offset_y < 0
                        or j + offset_y >= len(board[0])
                        or board[i + offset_x][j + offset_y] != 0
                    )
                ):
                    return True
        return False

    def move_down(self):
        if not self.check_collision(self.board, self.piece, self.posX + 1, self.posY):
            self.processIA()
            print("After IA")
            self.print_matrix(self.board)  
            self.posX += 1
        else:
            self.board = self.merge_matrices(self.board, self.piece, self.posX, self.posY)
            print("After merge_matrices")
            self.print_matrix(self.board) 
            self.clear_lines()
            self.piece = self.nextPiece
            self.nextPiece = self.get_random_piece()
            self.posX, self.posY = 0, 4

    def clear_lines(self):
        lines_to_clear = [i for i, row in enumerate(self.board) if all(row)]
        for i in lines_to_clear:
            del self.board[i]
            self.board.insert(0, [0] * 10)

    def game_loop(self):
        print("Start Game Loop")
        self.print_matrix(self.board)        
        self.move_down()
        print("End move")
        self.print_matrix(self.board)        
        board = self.merge_matrices(self.board, self.piece, self.posX, self.posY)
        self.draw(board)
        print("End Game Loop")
        self.print_matrix(self.board)
        self.window.after(self.sleep, self.game_loop)

    def processIA(self):
        print("Start IA")
        self.print_matrix(self.board)
        best_score = float('-inf')
        best_position = None
        best_piece = None
        last_viable_position = None

        rotated_piece = self.rotate_piece(self.piece)
        for rotation in range(4):
            rotated_piece = self.rotate_piece(rotated_piece)
            for y_pos in range(len(self.board[0]) + 1):
                for x_pos in range(len(self.board)):
                    if not self.check_collision(self.board, rotated_piece, x_pos, y_pos):
                        last_viable_position = (x_pos, y_pos)
                    else:
                        temp_frame = self.merge_matrices(self.board, rotated_piece, last_viable_position[0], last_viable_position[1])
                        # print("Temp frame:")
                        # self.print_matrix(temp_frame)
                        self.draw(temp_frame, color="red")
                        self.window.update()  # Force update to display the changes
                        #time.sleep(0.0)  # Adjust the sleep time as needed
                        score = self.evaluate_position(temp_frame, last_viable_position[0], y_pos)
                        if score > best_score:
                            best_score = score
                            best_position = last_viable_position
                            best_piece = rotated_piece
                        break

        # Update position only if there was a viable position before a collision
        if best_position is not None:
            self.posY = last_viable_position[1]
            self.piece = best_piece
        print("Fin IA")
        self.print_matrix(self.board)

    def print_matrix(self, matrix):
        print("*" * 30)
        for row in matrix:
            print(row)
        print("\n")
        
    def evaluate_position(self, current_board, x, y):
        score = x
        # temp_frame = self.merge_matrices(current_board, piece, x, y)

        # weight = 0
        # holes = 0
        # for i in range(len(temp_frame)):
        #     for j in range(len(temp_frame[i])):
        #         if temp_frame[i][j] != 0:
        #             weight += j * j
        #         else:
        #             if j > 0 and temp_frame[i][j - 1] != 0:
        #                 holes += 1
        # # score += x
        # # score -= holes
        # score += weight
        return score


def launch(datas):
    num_threads = 2
    if num_threads == 1:
        results = [Tetris(item) for item in datas]
    else:
        with ThreadPoolExecutor(max_workers=num_threads) as executor:
            futures = [executor.submit(Tetris, item) for item in datas]
        results = [future.result() for future in futures]


if __name__ == "__main__":
    window = tk.Tk()
    #window.attributes('-zoomed', True)
    window.update()

    width, height = 2, 2

    # Create a toolbar frame at the top
    toolbar_frame = tk.Frame(window)
    toolbar_frame.grid(row=0, column=0, columnspan=width, sticky="ew")
    button = tk.Button(toolbar_frame, text="Start process ...", command=lambda: launch(datas))
    button.pack(side=tk.LEFT, padx=5)

    window.mainloop()
