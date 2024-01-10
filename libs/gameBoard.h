extern FastLED_NeoMatrix *matrix; // Déclaration de la variable globale

class Coord {
public:
    int x;
    int y;
    Coord(int x, int y) : x(x), y(y) {}
};

class Board {
    private:
    public:
        int gameWidth; // Espace de jeu Hauteur
        int gameHeight;  // Espace de jeu Largeur
        int viewHeight; // Vue Hauteur
        int viewWidth;  // Vue Largeur
        int positionX = 0;  // Positionnement sur la matrice X
        int positionY = 0;  // Positionnement sur la matrice Y
        std::vector<std::vector<CRGB>> board;
        Board(int gameWidth, int gameHeight, CRGB Color, int viewHeight = 0, int viewWidth = 0) 
            :   gameWidth(gameWidth), 
                gameHeight(gameHeight), 
                viewHeight(viewHeight == 0 ? gameWidth : viewHeight),
                viewWidth(viewWidth == 0 ? gameHeight : viewWidth)
            {
            board.resize(gameWidth, std::vector<CRGB>(gameHeight, Color));                   
        }

        // Position of block board
        void position(int pos_X = 0, int pos_Y = 0){
            positionX = pos_X;
            positionY = pos_Y;
        }

        // Affiche le tableau en mémoire
        void show (boolean tranparent = true, int start_x = 0, int start_y = 0 ){
            // On override le focus pour exploiter l'espace disponible lorsqu'on est sur un bord
            // if ( start_x + viewWidth >= board.size() ) start_x = board.size() - viewWidth;
            // if ( start_y + viewHeight >= board[0].size() ) start_y = board[0].size() - viewHeight;
            showTemp(board,tranparent,start_x,start_y);
        }

        // Push d'un tableau relatif a la board 
        void showTemp(const std::vector<std::vector<CRGB>>& customBoard, bool transparent = true, int start_x = 0, int start_y = 0) {
            for (int i = 0; i < customBoard.size(); i++) {
                for (int j = 0; j < customBoard[i].size(); j++) {
                    if (transparent && customBoard[i][j] == CRGB::Black) continue;
                    matrix->drawPixel(i+positionX, j+positionY, customBoard[i][j]);      
                }
            }
        }

        // Remplir data + matrix
        void fill(CRGB Color) {
            for (int i = 0; i < board.size(); i++) {
               for (int j = 0; j < board[i].size(); j++) {
                    board[i][j] = Color;
                }
            }  
        }
};

// Detection une collision entre deux vectors
bool isCollisionDetected(const std::vector<std::vector<CRGB>>& largerVector, const std::vector<std::vector<CRGB>>& smallerVector, int x, int y) {
    // Les objets ne mappe pas
    if (x < 0 || y < 0 || x + smallerVector[0].size() > largerVector[0].size() || y + smallerVector.size() > largerVector.size()) {
        return true;
    }
    // Test data déja présente
    for (size_t i = 0; i < smallerVector.size(); ++i) {
        for (size_t j = 0; j < smallerVector[0].size(); ++j) {
            if ( smallerVector[i][j] == CRGB::Black ) continue;
            if ( largerVector[i + y][j + x] != CRGB::Black ) return true;
        }
    }
    return false;
}

std::vector<std::vector<CRGB>> mergeRGBVectors(const std::vector<std::vector<CRGB>>& largerVector, const std::vector<std::vector<CRGB>>& smallerVector, int x, int y) {
    // Créez le vecteur fusionné avec les dimensions appropriées
    std::vector<std::vector<CRGB>> mergedVector = largerVector;

    // Fusionnez les données de smallerVector dans mergedVector
    for (size_t i = 0; i < smallerVector.size(); ++i) {
        for (size_t j = 0; j < smallerVector[0].size(); ++j) {
            if (smallerVector[i][j] != CRGB::Black) {
                mergedVector[i + y][j + x] = smallerVector[i][j];
            }
        }
    }
    return mergedVector;
}

std::vector<std::vector<CRGB>> rotateMatrix90Clockwise(const std::vector<std::vector<CRGB>>& matrix) {
    std::vector<std::vector<CRGB>> result(matrix[0].size(), std::vector<CRGB>(matrix.size()));
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            result[j][matrix.size() - 1 - i] = matrix[i][j];
        }
    }
    return result;
}

std::vector<std::vector<CRGB>> rotateMatrix90AntiClockwise(const std::vector<std::vector<CRGB>>& inputMatrix) {
    // Obtenez les dimensions de la matrice d'entrée
    size_t numRows = inputMatrix.size();
    size_t numCols = inputMatrix[0].size();

    // Créez une nouvelle matrice pour le résultat en échangeant les dimensions
    std::vector<std::vector<CRGB>> rotatedMatrix(numCols, std::vector<CRGB>(numRows, CRGB::Black));

    // Copiez les données de la matrice d'entrée dans la matrice résultante en effectuant la rotation
    for (size_t i = 0; i < numRows; ++i) {
        for (size_t j = 0; j < numCols; ++j) {
            rotatedMatrix[numCols - 1 - j][i] = inputMatrix[i][j];
        }
    }

    return rotatedMatrix;
}

std::vector<std::vector<CRGB>> rotateMatrix180(const std::vector<std::vector<CRGB>>& inputMatrix) {
    // Obtenez les dimensions de la matrice d'entrée
    size_t numRows = inputMatrix.size();
    size_t numCols = inputMatrix[0].size();

    // Créez une nouvelle matrice pour le résultat
    std::vector<std::vector<CRGB>> rotatedMatrix(numRows, std::vector<CRGB>(numCols, CRGB::Black));

    // Inversez chaque ligne de la matrice d'entrée et placez-la à l'envers dans la matrice résultante
    for (size_t i = 0; i < numRows; ++i) {
        for (size_t j = 0; j < numCols; ++j) {
            rotatedMatrix[i][j] = inputMatrix[numRows - 1 - i][numCols - 1 - j];
        }
    }

    return rotatedMatrix;
}

std::vector<std::vector<CRGB>> reverseMatrixVertically(const std::vector<std::vector<CRGB>>& inputMatrix) {
    // Obtenez les dimensions de la matrice d'entrée
    size_t numRows = inputMatrix.size();
    size_t numCols = inputMatrix[0].size();

    // Créez une nouvelle matrice pour le résultat
    std::vector<std::vector<CRGB>> reversedMatrix(numRows, std::vector<CRGB>(numCols, CRGB::Black));

    // Inversez l'ordre des lignes de la matrice d'entrée
    for (size_t i = 0; i < numRows; ++i) {
        for (size_t j = 0; j < numCols; ++j) {
            reversedMatrix[numRows - 1 - i][j] = inputMatrix[i][j];
        }
    }

    return reversedMatrix;
}

std::vector<std::vector<CRGB>> reverseMatrixHorizontally(const std::vector<std::vector<CRGB>>& inputMatrix) {
    // Obtenez les dimensions de la matrice d'entrée
    size_t numRows = inputMatrix.size();
    size_t numCols = inputMatrix[0].size();

    // Créez une nouvelle matrice pour le résultat
    std::vector<std::vector<CRGB>> reversedMatrix(numRows, std::vector<CRGB>(numCols, CRGB::Black));

    // Inversez l'ordre des colonnes de la matrice d'entrée
    for (size_t i = 0; i < numRows; ++i) {
        for (size_t j = 0; j < numCols; ++j) {
            reversedMatrix[i][numCols - 1 - j] = inputMatrix[i][j];
        }
    }

    return reversedMatrix;
}

CRGB GetRandomColor() {
    uint8_t red = random(256);
    uint8_t green = random(256);
    uint8_t blue = random(256);
    return CRGB(red, green, blue);
}