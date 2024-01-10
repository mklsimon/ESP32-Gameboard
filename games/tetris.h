extern FastLED_NeoMatrix *matrix; // Déclaration de la variable globale

std::vector<std::vector<CRGB>> tetrisI = {
  {tetrisColor1, tetrisColor1, tetrisColor1, tetrisColor1}
};

std::vector<std::vector<CRGB>> tetrisL = {
  {emptyColor, emptyColor, tetrisColor2},
  {tetrisColor2, tetrisColor2, tetrisColor2},
};

std::vector<std::vector<CRGB>> tetrisSquare = {
  {tetrisColor3, tetrisColor3},
  {tetrisColor3, tetrisColor3}
};

std::vector<std::vector<CRGB>> tetrisT = {
  {tetrisColor4, tetrisColor4, tetrisColor4},
  {emptyColor, tetrisColor4, emptyColor}
};

std::vector<std::vector<CRGB>> tetrisLInverse = {
  {tetrisColor5, tetrisColor5, tetrisColor5},
  {emptyColor, emptyColor, tetrisColor5}
};

std::vector<std::vector<CRGB>> tetrisZ = {
  {tetrisColor6, tetrisColor6, emptyColor},
  {emptyColor, tetrisColor6, tetrisColor6}
};

std::vector<std::vector<CRGB>> tetrisS = {
  {emptyColor, tetrisColor7, tetrisColor7},
  {tetrisColor7, tetrisColor7, emptyColor}
};

// Liste de toutes les pièces Tetris
std::vector<std::vector<std::vector<CRGB>>*> pieces = {&tetrisI, &tetrisL, &tetrisSquare, &tetrisT, &tetrisLInverse, &tetrisZ, &tetrisS};

class Tetris {
private:
  int numPlayers;  // Le nombre de joueurs
  std::vector<Board> tetrisBoards;   // Un vecteur de Board pour chaque joueur
  std::vector<Board> nextPieces;     // Un vecteur de Board pour les prochaines pièces de chaque joueur
  std::vector<Board> currentPiece;   // Un vecteur de Board pour chaque joueur
  std::vector<int> playerXPositions;  // Les positions "x" de la pièce actuelle pour chaque joueur
  std::vector<int> playerYPositions;  // Les positions "y" de la pièce actuelle pour chaque joueur
  std::vector<long> playerFallTime;  // Les timer des joueurs pour chute de piece
  std::vector<long> playerMoveTime;  // Les timer des joueurs pour mouvement
  std::vector<int> playerMove;   // Demande de deplacement
  std::vector<int> playerRotate; // Demande de rotation
  std::vector<int> playerScore;  // Tableau des scores
  std::vector<int> playerMalus;  // Tableau des Malus
  std::vector<int> iaTargetMove;  // Meilleur position IA
  std::vector<int> PlayerStatus;  // Status du joueur
  int loopFall = 50; // Temps en ms minimum d'un loop - mouvement piece
  int loopMove = 50; // Temps en ms minimum d'un loop - mouvements joueurs

public:
    Tetris(int num) : numPlayers(num) {
        // Initialisation de la classe Tetris avec le nombre de joueurs spécifié
        tetrisBoards.resize(numPlayers, Board(10, 23, emptyColor)); // Crée un tableau de Boards pour chaque joueur 
        nextPieces.resize(numPlayers, Board(4, 4, emptyColor));     // Crée un tableau de Boards pour les prochaines pièces de chaque joueur
        currentPiece.resize(numPlayers, Board(4, 4, emptyColor));   // Crée un tableau de Boards pour la piece courrante
        playerXPositions.resize(numPlayers, 0);  // Initialise les positions "x" pour chaque joueur à zéro
        playerYPositions.resize(numPlayers, 4);  // Initialise les positions "y"  # Posiiton de la premier brique
        playerFallTime.resize(numPlayers, millis());  // Temps minimum par loop
        playerMoveTime.resize(numPlayers, millis());  // Temps minimum par loop
        playerMove.resize(numPlayers, 0); // Mouvement Gauche
        playerRotate.resize(numPlayers, 0); // Mouvement Gauche
        playerScore.resize(numPlayers, 0); // Score des joeurus
        playerMalus.resize(numPlayers, 0); // Malus - lignes a ajouter
        PlayerStatus.resize(numPlayers, 1); // Malus - lignes a ajouter
        iaTargetMove.resize(numPlayers, -1); // Malus - lignes a ajouter

        // Distribution des blocs aux joueurs
        for (int i = 0; i < numPlayers; i++) {
            nextPieces[i].board=randomPiece();
            // if ( numPlayers < 3){
                tetrisBoards[i].position( i * 16 + 1, 9);
                matrix->drawLine(tetrisBoards[i].positionX-1, tetrisBoards[i].positionY-1, tetrisBoards[i].positionX-1,tetrisBoards[i].positionY + tetrisBoards[i].gameHeight, matrix->Color(255, 255, 255)); // vertical gauche
                matrix->drawLine(tetrisBoards[i].positionX-1, tetrisBoards[i].positionY-1, tetrisBoards[i].positionX + tetrisBoards[i].gameWidth, tetrisBoards[i].positionY-1, matrix->Color(255, 255, 255)); // horizontal
                matrix->drawLine(tetrisBoards[i].positionX-1, tetrisBoards[i].positionY + tetrisBoards[i].gameHeight, tetrisBoards[i].positionX + tetrisBoards[i].gameWidth, tetrisBoards[i].positionY + tetrisBoards[i].gameHeight, matrix->Color(255, 255, 255)); // horizontal
                matrix->drawLine(tetrisBoards[i].positionX + tetrisBoards[i].gameWidth, tetrisBoards[i].positionY-1, tetrisBoards[i].positionX + tetrisBoards[i].gameWidth, tetrisBoards[i].positionY + tetrisBoards[i].gameHeight, matrix->Color(255, 255, 255)); // vertical
                nextPieces[i].position( tetrisBoards[i].positionX + tetrisBoards[i].gameWidth + 2 ,tetrisBoards[i].positionY + 2 );
                showScore(tetrisBoards[i].positionX-1, 0 ,playerScore[i]);
                
            // } else {
            //     tetrisBoards[i].position( i * 11, 8);
            //     matrix->drawLine(tetrisBoards[i].positionX, tetrisBoards[i].positionY-1, tetrisBoards[i].positionX + tetrisBoards[i].gameWidth, tetrisBoards[i].positionY-1, matrix->Color(255, 255, 255)); // horizontal
            //     matrix->drawLine(tetrisBoards[i].positionX, tetrisBoards[i].positionY + tetrisBoards[i].gameHeight, tetrisBoards[i].positionX + tetrisBoards[i].gameWidth, tetrisBoards[i].positionY + tetrisBoards[i].gameHeight, matrix->Color(255, 255, 255)); // horizontal
            //     matrix->drawLine(tetrisBoards[i].positionX + tetrisBoards[i].gameWidth, tetrisBoards[i].positionY-1, tetrisBoards[i].positionX + tetrisBoards[i].gameWidth, tetrisBoards[i].positionY + tetrisBoards[i].gameHeight, matrix->Color(255, 255, 255)); // vertical
            //     nextPieces[i].position( i * 11 + 5, 2);
            // }
            nextPieces[i].show(false);
            currentPiece[i].board=randomPiece();
        }
        matrix->show();
    }    
  
  void setup() {
    randomSeed(analogRead(0));
  }

  // Boucle de la class
    boolean loop() {
        int status = 0;
        for (int index = 0; index < tetrisBoards.size(); index++) {
            status += PlayerStatus[index];
        }
        if (status == 0) return false;

        // Joueur par joueur
        for (int index = 0; index < tetrisBoards.size(); index++) {
            if (PlayerStatus[index] == 0){
                // tetrisBoards[index].fill(CRGB::Yellow);
                // tetrisBoards[index].show();
                continue;
            }
            // Tempo mouvement de joueurs
            if ( millis() - playerMoveTime[index] < loopMove ) continue;
            playerMoveTime[index] = millis();

            // On applique le malus venant des autres joueurs
            if (playerMalus[index] > 0){
                tetrisBoards[index].board = addRows(tetrisBoards[index].board,playerMalus[index]);
                playerMalus[index] = 0 ;
                tetrisBoards[index].show();
            }

            if ( iaTargetMove[index] == -1 ){
                iaTargetMove[index] = findBestMove(index);
            };

            if ( iaTargetMove[index] > playerYPositions[index] ) playerMove[index] = 1;
            else if ( iaTargetMove[index] < playerYPositions[index] ) playerMove[index] = -1;
            else {
                playerMoveTime[index] = 0;
                playerFallTime[index] = 0;
            }

            // Traitement des entrées BT
            // ControllerInfo pad = BT.getControllerStatus(index);
            // if (pad.ONLINE){
            //     if ( pad.LEFT ) playerMove[index] = -1;
            //     else if ( pad.RIGHT ) playerMove[index] = 1;
            //     if ( pad.DOWN ) {
            //         playerMoveTime[index] = 0;
            //         playerFallTime[index] = 0;
            //     }
            //     else if ( pad.CIRCLE) playerRotate[index] = 1;
            //     else if ( pad.SQUARE) playerRotate[index] = -1;
            //     else if ( pad.CROSS ) playerRotate[index] = 1;
            //     else if ( pad.TRIANGLE ) playerRotate[index] = -1;
            //     else if ( pad.PS3 ) return false;
            // }

            // Rotation
            if (playerRotate[index] != 0){
                if ( playerRotate[index] == -1 && !(isCollisionDetected(tetrisBoards[index].board,rotateMatrix90Clockwise(currentPiece[index].board),playerXPositions[index],playerYPositions[index]))) {
                    currentPiece[index].board = rotateMatrix90Clockwise(currentPiece[index].board);
                }
                else if ( playerRotate[index] == 1 && !(isCollisionDetected(tetrisBoards[index].board,rotateMatrix90AntiClockwise(currentPiece[index].board),playerXPositions[index],playerYPositions[index]))) {
                    currentPiece[index].board = rotateMatrix90AntiClockwise(currentPiece[index].board);
                }
                playerRotate[index] = 0;
            }

            // Gauche / Droite
            if (playerMove[index] != 0 ){
                if ( !(isCollisionDetected(tetrisBoards[index].board,currentPiece[index].board,playerXPositions[index],playerYPositions[index]+playerMove[index]))) {
                    playerYPositions[index] += playerMove[index];
                }
                playerMove[index] = 0;
            }

            // Build du display - Fusion de la board et de la piece relatif
            std::vector<std::vector<CRGB>> frame = mergeRGBVectors(tetrisBoards[index].board,currentPiece[index].board,playerXPositions[index],playerYPositions[index]);
            tetrisBoards[index].showTemp(frame,false);

            // Tempo avant de descendre d'une ligne
            if ( millis() - playerFallTime[index] < loopFall ) continue;
            playerFallTime[index] = millis();

            // Descendre d'une ligne & lignes completes
            if ( isCollisionDetected(tetrisBoards[index].board,currentPiece[index].board,playerXPositions[index]+1,playerYPositions[index]) ){
                // Fin du jeu on reset la board
                if (playerXPositions[index] == 0) {
                    PlayerStatus[index] = 0;
                }

                // Traitement des lignes completes
                int lines = 0;
                for (int lineIndex : findCompleteLines(frame)) {
                    matrix->drawLine(tetrisBoards[index].positionX, tetrisBoards[index].positionY + lineIndex,
                                    tetrisBoards[index].positionX + tetrisBoards[index].gameWidth - 1,
                                    tetrisBoards[index].positionY + lineIndex, matrix->Color(255, 255, 255));

                    frame = removeRow(frame, lineIndex);
                    lines++;
                }

                if (lines > 0) {
                    // Gestion du score
                    playerScore[index] += lines;
                    showScore(tetrisBoards[index].positionX-1, 0 ,playerScore[index]);

                    // Pénalité pour les adversaires
                    for (int player = 0; player < tetrisBoards.size(); player++) {
                        if (index != player) {
                            //playerMalus[player] += lines - 1;
                        }
                    }
                }
                // On stocke la frame actuelle
                tetrisBoards[index].board = frame;
                currentPiece[index].board = nextPieces[index].board;

                // On efface la nextPiece actuel
                nextPieces[index].fill(emptyColor);
                nextPieces[index].show(false);

                // On prepare la nextPiece
                nextPieces[index].board=randomPiece();   
                nextPieces[index].show();
                playerXPositions[index] = 0;
                playerYPositions[index] = 4;
                iaTargetMove[index] = -1;
                
            } else {
                // la piece descend
                playerXPositions[index]++;
            }
        }
        matrix->show();    
        return true; // Pour continuer le loop
    }

    // Tirer une piece au hasard
    std::vector<std::vector<CRGB>> randomPiece(){
        int pieceIndex = random(pieces.size());
        std::vector<std::vector<CRGB>> piece = *pieces[pieceIndex];
        return piece;
    }

    // Identifier une ligne complete
    std::vector<int> findCompleteLines(const std::vector<std::vector<CRGB>>& vectorOfVectors) {
        std::vector<int> resultLines;
        for (int i = 0; i < vectorOfVectors[0].size(); i++) {
            bool lineContainsEmpty = false;
            for (int j = 0; j < vectorOfVectors.size(); j++) {
                if (vectorOfVectors[j][i] == CRGB::Black) { lineContainsEmpty = true; break; }
            }
            if (lineContainsEmpty == false ) {
                resultLines.push_back(i);
            }
        }  
        return resultLines;
    }

    // Fonction pour supprimer une ligne d'un vecteur de vecteurs en fonction de l'indice de la ligne et retourner un nouveau vecteur sans cette ligne
    std::vector<std::vector<CRGB>> removeRow(const std::vector<std::vector<CRGB>>& vectorOfVectors, size_t rowIndex) {
        std::vector<std::vector<CRGB>> newMatrix = rotateMatrix90Clockwise(vectorOfVectors);
        newMatrix.erase(newMatrix.begin() + rowIndex);
        std::vector<CRGB> blackLine(newMatrix[rowIndex].size(), CRGB::Black);
        newMatrix.insert(newMatrix.begin(), blackLine);
        newMatrix = rotateMatrix90AntiClockwise(newMatrix);
        return newMatrix;
    }

    // Fonction pour insérer plusieurs lignes en bas du tableau
    std::vector<std::vector<CRGB>> addRows(const std::vector<std::vector<CRGB>>& vectorOfVectors, int lines) {
        std::vector<std::vector<CRGB>> newMatrix = rotateMatrix90Clockwise(vectorOfVectors);
        for (int i = 0; i < lines; ++i) {
            newMatrix.erase(newMatrix.begin());
            // Ligne aléatoire
            std::vector<CRGB> malusLine(newMatrix[0].size(), CRGB::Red);
            size_t randomIndex = random(malusLine.size());
            malusLine[randomIndex] = CRGB::Black;
            newMatrix.emplace_back(malusLine);
        }        
        newMatrix = rotateMatrix90AntiClockwise(newMatrix);
        return newMatrix;
    }

    // Affichage du score
    void showScore(int x, int y, int score){
        matrix->fillRect(x,y, 16, 7, matrix->Color(0, 0, 0));
        matrix->setCursor(x, y );
        matrix->print(String(score));
    }

    int findBestMove(int playerIndex) {
        int last_x ;
        int last_y ;
        int score = 0;
        int bestScore = 0;
        int best_yPos = playerYPositions[playerIndex];
        std::vector<std::vector<CRGB>> rotatedPiece = currentPiece[playerIndex].board;
        std::vector<std::vector<CRGB>> bestPiece = currentPiece[playerIndex].board;
        for (int rotation = 0; rotation < 4; ++rotation) {
            for (int yPos = 0; yPos < tetrisBoards[playerIndex].gameWidth; ++yPos) {
                for (int xPos = 0; xPos < tetrisBoards[playerIndex].gameHeight; ++xPos) {
                    if (!isCollisionDetected(tetrisBoards[playerIndex].board, rotatedPiece, xPos, yPos)) {
                        last_x = xPos;
                        last_y = yPos;
                    } else {
                        int score = evaluatePosition(tetrisBoards[playerIndex].board, rotatedPiece, last_x, last_y, playerIndex);
                        if (score > bestScore) {
                            bestScore = score;
                            best_yPos = last_y;
                            bestPiece = rotatedPiece;
                        }
                        break;
                    }
                }
            }
            rotatedPiece = rotateMatrix90Clockwise(rotatedPiece);
        }
        currentPiece[playerIndex].board = bestPiece;
        //playerYPositions[playerIndex] = best_yPos;
        return best_yPos;
    }
    int evaluatePosition(const std::vector<std::vector<CRGB>>& currentBoard, const std::vector<std::vector<CRGB>>& piece, int x, int y, int playerIndex) {
        // Score de base la position de piece
        int score = x;
        
        // Fusion de la pièce avec le plateau actuel
        std::vector<std::vector<CRGB>> tempFrame = mergeRGBVectors(currentBoard, piece, x, y );

        // Recherche de lignes complètes
        // std::vector<int> completeLines = findCompleteLines(tempFrame);
        // if (!completeLines.empty()) {
        //     score *= 10;
        // }
        int Weight = 0;
        int Holes = 0;
        // Calcul d'un poids pour la board - plus les pieces sont en bas plus le poids est elevé
        // Calcul du nombre de trous
        for (int i = 0; i < tempFrame.size(); ++i) {
            for (int j = 0; j < tempFrame[i].size(); ++j) {
                if (tempFrame[i][j] != CRGB::Black ) {
                    Weight += j;
                } else {
                    if (j > 0 && (tempFrame[i][j - 1] != CRGB::Black )){
                        Holes += 1;
                    }
                }
            }
        }
        score -= Holes * 20;
        score += Weight ;

        //Pénaliser les trous potentiels dans le score
        return score;
    }
};


