#!/home/adm-misimon/venv/bin/python
import time
import random
import tkinter as tk
from tkinter import ttk
from concurrent.futures import ThreadPoolExecutor
import copy 
from functools import lru_cache

# datas = [{"coeff_trous": 2000, "coeff_poids": 10},{"coeff_trous": 2000, "coeff_poids": 50},{"coeff_trous": 2000, "coeff_poids": 1000},{"coeff_trous": 20, "coeff_poids": 500}]
# # {"coeff_trous": 500, "coeff_poids": 500},{"coeff_trous": 50, "coeff_poids": 50},{"coeff_trous": 50, "coeff_poids": 10},{"coeff_trous": 10, "coeff_poids": 50}
# # ]

global_scores =[{"coeff_trous": 0, "coeff_poids": 0, 1: 0, 2: 0, 3: 0, 4: 0, "total_score": 0,"lines":0}]


def generate_all_configurations(step=10):
    configurations = []
    max_value = 1000  # Adjust this as needed

    for coeff_trous in range(1, max_value + 1, step):
        for coeff_poids in range(1, max_value + 1, step):
            config = {
                "coeff_trous": coeff_trous,
                "coeff_poids": coeff_poids
            }
            configurations.append(config)
    return configurations

datas = generate_all_configurations(200)
print(len(datas))

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
        best_piece = self.cache_position( board, current_piece)
        self.shape = best_piece.shape
        self.X = best_piece.X
        self.Y = best_piece.Y
        return self.Y, best_piece.shape

    @lru_cache
    def cache_position(self, board, current_piece):
        best_score = 0
        last_piece = None
        best_piece = copy.deepcopy(current_piece)
        piece = TetrisPiece(index=current_piece.index)
        for rotation in range(4):
            piece.rotate_piece()
            for y_pos in range(board.width):
                for x_pos in range(board.height):                
                    piece.set_position(x_pos, y_pos)
                    if not board.collision(piece, x_pos, y_pos):                        
                        last_piece = copy.deepcopy(piece)
                        score = self.get_score(board, last_piece)
                        if score > best_score:
                            best_score = score
                            best_piece = last_piece
                    else:
                        break
        return copy.deepcopy(best_piece)

    @lru_cache
    def get_score(self, board, piece):
        tmpBoard = board.merge_piece(piece)
        score = piece.X
        weight = 0
        holes = 0
        for i in range(len(tmpBoard.shape)):
            for j in range(len(tmpBoard.shape[i])):
                if tmpBoard.shape[i][j] != 0:
                    weight += i * i 
                else:
                    if i > 0 and tmpBoard.shape[i - 1][j] != 0:
                        holes += 1
        score += weight * self.configIA['coeff_poids']
        score -= holes * self.configIA['coeff_trous']
        return score
    
class TetrisPiece:
    def __init__(self, pos_x=0, pos_y=4, index=None):
        if index is not None:
            self.shape = tetris_piece[index]
            self.index = index
        else:
            self.shape, self.index = self.get_random_piece()
        self.width, self.height = self.size_it()
        self.X = pos_x
        self.Y = pos_y

    def set_position(self,X,Y):
        self.X, self.Y = X, Y

    def get_random_piece(self):
        piece_index = random.randint(0, len(tetris_piece) - 1)
        return tetris_piece[piece_index], piece_index
    
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
        self.loop = 10

        # Config display
        self.configIA = configIA
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
        self.sleep = 1
        self.scores = {**{1: 0, 2: 0, 3: 0, 4: 0, "total_score": 0,"lines":0},**configIA}

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
                global_scores.append(self.scores)
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
        #tempBoard = self.board.merge_piece(self.piece,2)
        #tempBoard = tempBoard.merge_piece(self.piece,2)(self.ai,3)
        #self.draw(self.board.shape)
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
            futures = [executor.submit(run_tetris, item) for item in datas]
        results = [future.result() for future in futures]
    print(results)
    window.after(1000, update_scores_table)

def run_tetris(item):
    tetris_instance = Tetris(item)
    return tetris_instance.scores

def update_scores_table():
    # Obtenez toutes les clés uniques des configurations
    all_keys = set()
    for scores_data in global_scores:
        all_keys.update(scores_data.keys())

    # Supprimez le tableau existant s'il y en a un
    for widget in main_frame.winfo_children():
        widget.destroy()

    tableau = ttk.Treeview(main_frame)
    tableau["columns"] = tuple(all_keys)
    tableau.heading("#0", text="Configuration")
    tableau.column("#0", anchor="w", width=100)

    for col in tableau["columns"]:
        tableau.heading(col, text=col, command=lambda c=col: sort_table(tableau, c, c == "total_score"))
        tableau.column(col, anchor="center", width=100)

    # Insérez les données dans le tableau
    for idx, scores_data in enumerate(global_scores):
        config_text = f"Config {idx + 1}"
        
        # Vérifiez si un enregistrement pour cette configuration existe déjà
        existing_item = None
        for item_id in tableau.get_children():
            item_text = tableau.item(item_id, "text")
            if item_text == config_text:
                existing_item = item_id
                break

        if existing_item is not None:
            # Mettez à jour les valeurs existantes
            values = [scores_data.get(key, 0) for key in all_keys]
            tableau.item(existing_item, values=tuple(values))
        else:
            # Ajoutez un nouvel enregistrement
            values = [scores_data.get(key, 0) for key in all_keys]
            tableau.insert("", idx, text=config_text, values=tuple(values))

    # Emballez le tableau dans le frame
    tableau.pack(expand=True, fill=tk.BOTH)



def sort_table(tree, col, reverse):
    # Fonction de tri pour la colonne spécifiée
    l = [(tree.set(k, col), k) for k in tree.get_children('')]
    l.sort(reverse=reverse)

    # Tri selon le type de données (numérique ou alphabétique)
    try:
        l = [(float(item[0]), item[1]) for item in l]
    except ValueError:
        pass

    for index, (val, k) in enumerate(l):
        tree.move(k, '', index)

    # Inverse la direction pour la prochaine fois.
    tree.heading(col, command=lambda: sort_table(tree, col, not reverse))


if __name__ == "__main__":
    window = tk.Tk()
    width, height = 2, 2
    # Create a toolbar frame at the top using grid
    toolbar_frame = tk.Frame(window)
    toolbar_frame.grid(row=0, column=0, columnspan=width, sticky="ew")
    button = tk.Button(toolbar_frame, text="Start process ...", command=lambda: launch(datas))
    button.grid(row=0, column=0, padx=5)

    main_frame = tk.Frame(window)
    main_frame.grid(row=1, column=0, columnspan=width, sticky="nsew")

    window.after(1000, update_scores_table)

    window.mainloop()
