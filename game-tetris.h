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
  std::vector<int> playerMove; // Demande de deplacement
  std::vector<int> playerRotate; // Demande de rotation
  std::vector<int> playerScore; // Tableau des scores
  std::vector<int> playerMalus; // Tableau des Malus
  int loopFall = 500; // Temps en ms minimum d'un loop - mouvement piece
  int loopMove = 50; // Temps en ms minimum d'un loop - mouvements joueurs

public:
    Tetris(int num) : numPlayers(num) {
        // Initialisation de la classe Tetris avec le nombre de joueurs spécifié
        tetrisBoards.resize(numPlayers, Board(10, 22, emptyColor)); // Crée un tableau de Boards pour chaque joueur 
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

        // Distribution des blocs aux joueurs
        for (int i = 0; i < numPlayers; i++) {
            nextPieces[i].board=randomPiece();
            if ( numPlayers < 3){
                tetrisBoards[i].position( i * 16 + 1, 8);
                matrix->drawLine(tetrisBoards[i].positionX-1, tetrisBoards[i].positionY-1, tetrisBoards[i].positionX-1,tetrisBoards[i].positionY + tetrisBoards[i].gameWidth, matrix->Color(255, 255, 255)); // vertical gauche
                matrix->drawLine(tetrisBoards[i].positionX-1, tetrisBoards[i].positionY-1, tetrisBoards[i].positionX + tetrisBoards[i].gameHeight, tetrisBoards[i].positionY-1, matrix->Color(255, 255, 255)); // horizontal
                matrix->drawLine(tetrisBoards[i].positionX-1, tetrisBoards[i].positionY + tetrisBoards[i].gameWidth, tetrisBoards[i].positionX + tetrisBoards[i].gameHeight, tetrisBoards[i].positionY + tetrisBoards[i].gameWidth, matrix->Color(255, 255, 255)); // horizontal
                matrix->drawLine(tetrisBoards[i].positionX + tetrisBoards[i].gameHeight, tetrisBoards[i].positionY-1, tetrisBoards[i].positionX + tetrisBoards[i].gameHeight, tetrisBoards[i].positionY + tetrisBoards[i].gameWidth, matrix->Color(255, 255, 255)); // vertical
                nextPieces[i].position( tetrisBoards[i].positionX + tetrisBoards[i].gameHeight + 1 ,tetrisBoards[i].positionY + 1 );
                showScore(tetrisBoards[i].positionX-1, tetrisBoards[i].positionY - 8 ,playerScore[i]);
                
            }else {
                tetrisBoards[i].position( i * 11, 8);
                matrix->drawLine(tetrisBoards[i].positionX, tetrisBoards[i].positionY-1, tetrisBoards[i].positionX + tetrisBoards[i].gameHeight, tetrisBoards[i].positionY-1, matrix->Color(255, 255, 255)); // horizontal
                matrix->drawLine(tetrisBoards[i].positionX, tetrisBoards[i].positionY + tetrisBoards[i].gameWidth, tetrisBoards[i].positionX + tetrisBoards[i].gameHeight, tetrisBoards[i].positionY + tetrisBoards[i].gameWidth, matrix->Color(255, 255, 255)); // horizontal
                matrix->drawLine(tetrisBoards[i].positionX + tetrisBoards[i].gameHeight, tetrisBoards[i].positionY-1, tetrisBoards[i].positionX + tetrisBoards[i].gameHeight, tetrisBoards[i].positionY + tetrisBoards[i].gameWidth, matrix->Color(255, 255, 255)); // vertical
                nextPieces[i].position( i * 11 + 5, 2);
            }
            nextPieces[i].show();
            currentPiece[i].board=randomPiece();
            tetrisBoards[i].showTemp(mergeRGBVectors(tetrisBoards[i].board,currentPiece[i].board,playerXPositions[i],playerYPositions[i]));            
        }
        matrix->show();
        delay(1000);

    }    
  
  void setup() {
    randomSeed(analogRead(0));
  }

  // Boucle de la class
  boolean loop() {
    // Traitement des entrées du webControler pour tous les joeurs
    for (int index = 0; index < tetrisBoards.size(); index++) {
        if (webController.gotMessages(index)){
            Serial.println("Message en attente !");
            String command = webController.getMessages(index);
            if ( command == "left") playerMove[index] = -1;
            else if ( command == "right") playerMove[index] = 1;
            // Accélartion de la descente
            else if ( command == "down") {
                playerMoveTime[index] = 0;
                playerFallTime[index] = 0;
            }
            // Rotation - YBGR
            else if ( command == "Y") playerRotate[index] = 1;
            else if ( command == "B") playerRotate[index] = -1;
            else if ( command == "G") playerRotate[index] = 1;
            else if ( command == "R") playerRotate[index] = -1;
            else if ( command == "exit") return false;

        }
    }

    for (int index = 0; index < tetrisBoards.size(); index++) {
        // Tempo mouvement de joueurs
        if ( millis() - playerMoveTime[index] < loopMove ) continue;
        playerMoveTime[index] = millis();
        // mouvements du joueurs

        // Rotation gauche / Droite
        if (playerRotate[index] != 0){
            if ( playerRotate[index] == -1 && !(isCollisionDetected(tetrisBoards[index].board,rotateMatrix90Clockwise(currentPiece[index].board),playerXPositions[index],playerYPositions[index]))) {
                currentPiece[index].board = rotateMatrix90Clockwise(currentPiece[index].board);
            }
            else if ( playerRotate[index] == 1 && !(isCollisionDetected(tetrisBoards[index].board,rotateMatrix90AntiClockwise(currentPiece[index].board),playerXPositions[index],playerYPositions[index]))) {
                currentPiece[index].board = rotateMatrix90AntiClockwise(currentPiece[index].board);
            }
            playerRotate[index] = 0;
        }

        // Déplacement a Gauche / Droite
        if (playerMove[index] != 0 ){
            if ( !(isCollisionDetected(tetrisBoards[index].board,currentPiece[index].board,playerXPositions[index],playerYPositions[index]+playerMove[index]))) {
                playerYPositions[index] += playerMove[index];
            }
            playerMove[index] = 0;
        }

        std::vector<std::vector<CRGB>> frame = mergeRGBVectors(tetrisBoards[index].board,currentPiece[index].board,playerXPositions[index],playerYPositions[index]);
        tetrisBoards[index].showTemp(frame,false);

        // Tempo avant de descendre d'une ligne
        if ( millis() - playerFallTime[index] < loopFall ) continue;
        playerFallTime[index] = millis();

        // Descendre d'une ligne
        if ( isCollisionDetected(tetrisBoards[index].board,currentPiece[index].board,playerXPositions[index]+1,playerYPositions[index]) ){
            // Fin du jeu on reset la board
            if (playerXPositions[index] == 0) {
                tetrisBoards[index].fill(CRGB::Black); 
                frame = tetrisBoards[index].board;
            }

            // Traitement des lignes completes
            int lines = 0;
            while (true) {
                std::vector<int> completedLines = findCompleteLines(frame);
                if (completedLines.empty()) {
                    // Gestion du score
                    playerScore[index]+=lines;
                    showScore(tetrisBoards[index].positionX -1 , tetrisBoards[index].positionY - 8 ,playerScore[index]);
                    // Pénalité pour les adversaires
                    for (int player = 0; player < tetrisBoards.size(); player++) {
                        if (index == player) continue;
                        if (lines > 1) playerMalus[player] += lines - 1 ;
                    }
                    break;
                }
                lines++;
                int line = completedLines.front();
                matrix->drawLine(tetrisBoards[index].positionX , tetrisBoards[index].positionY + line, tetrisBoards[index].positionX + tetrisBoards[index].gameHeight - 1 , tetrisBoards[index].positionY + line, matrix->Color(255, 255, 255));
                frame = removeRow(frame,line);
            }
            tetrisBoards[index].board = frame;

            // On applique le malus venant des autres joueurs
            if (playerMalus[index] > 0){
                tetrisBoards[index].board = addRows(frame,playerMalus[index]);
                playerMalus[index] = 0 ;
            }

            currentPiece[index].board = nextPieces[index].board;
            // On prepare les prochaines pieces
            nextPieces[index].board=randomPiece();
            nextPieces[index].resetLEDs(CRGB::Black);
            nextPieces[index].show();
            playerXPositions[index] = 0;
            playerYPositions[index] = 4;

        } else {
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
        matrix->setCursor(x, y + 4);
        matrix->print(String(score));
    }

};