extern FastLED_NeoMatrix *matrix; // Déclaration de la variable globale

class Board {
    private:
    public:
        int gameHeight; // Espace de jeu Hauteur
        int gameWidth;  // Espace de jeu Largeur
        int viewHeight; // Vue Hauteur
        int viewWidth;  // Vue Largeur
        int positionX = 0;  // Positionnement sur la matrice X
        int positionY = 0;  // Positionnement sur la matrice Y
        std::vector<std::vector<CRGB>> board;
        Board(int gameHeight, int gameWidth, CRGB Color, int viewHeight = 0, int viewWidth = 0) 
            :   gameHeight(gameHeight), 
                gameWidth(gameWidth), 
                viewHeight(viewHeight == 0 ? gameHeight : viewHeight),
                viewWidth(viewWidth == 0 ? gameWidth : viewWidth)
            {
            board.resize(gameHeight, std::vector<CRGB>(gameWidth, Color));                   
        }

        // Position of block board
        void position(int pos_X = 0, int pos_Y = 0){
            positionX = pos_X;
            positionY = pos_Y;
        }

        // Affiche le tableau en mémoire
        void show (boolean tranparent = true){
            for (int i = 0; i < board.size(); i++) {
                for (int j = 0; j < board[i].size(); j++) {
                    if ( tranparent && board[i][j] == CRGB::Black ) continue;
                    matrix->drawPixel(i+positionX, j+positionY, board[i][j]);
                }
            } 
        }

        // Push d'un tableau au coordonnée de l'objet
        void showTemp(const std::vector<std::vector<CRGB>>& customBoard, bool transparent = true) {
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
                    matrix->drawPixel(i+positionX, j+positionY, Color);
                    board[i][j] = Color;
                }
            }  
        }

        // 
        void resetLEDs(CRGB Color){
            for (int i = 0; i < gameHeight; i++) {
                for (int j = 0; j < gameWidth; j++) {
                    matrix->drawPixel(i+positionX, j+positionY, Color);
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
    std::vector<std::vector<CRGB>> mergedVector(largerVector.size(), std::vector<CRGB>(largerVector[0].size(),CRGB::Black));

    // Copiez les données de largerVector dans mergedVector
    for (size_t i = 0; i < largerVector.size(); ++i) {
        for (size_t j = 0; j < largerVector[0].size(); ++j) {
            mergedVector[i][j] = largerVector[i][j];
        }
    }

    // Fusionnez les données de smallerVector dans mergedVector
    for (size_t i = 0; i < smallerVector.size(); ++i) {
        for (size_t j = 0; j < smallerVector[0].size(); ++j) {
            if ( smallerVector[i][j] == CRGB::Black ) continue;
            mergedVector[i + y][j + x] = smallerVector[i][j];
        }
    }

    return mergedVector;
}

std::vector<std::vector<CRGB>> rotateMatrix90Clockwise(const std::vector<std::vector<CRGB>>& inputMatrix) {
    // Obtenez les dimensions de la matrice d'entrée
    size_t numRows = inputMatrix.size();
    size_t numCols = inputMatrix[0].size();

    // Créez une nouvelle matrice pour le résultat en échangeant les dimensions
    std::vector<std::vector<CRGB>> rotatedMatrix(numCols, std::vector<CRGB>(numRows, CRGB::Black));

    // Copiez les données de la matrice d'entrée dans la matrice résultante en effectuant la rotation
    for (size_t i = 0; i < numRows; ++i) {
        for (size_t j = 0; j < numCols; ++j) {
            rotatedMatrix[j][numRows - 1 - i] = inputMatrix[i][j];
        }
    }

    return rotatedMatrix;
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