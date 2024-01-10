extern FastLED_NeoMatrix *matrix; // Déclaration de la variable globale

// Enumération pour représenter la direction
enum class Direction {
    Up,
    Down,
    Left,
    Right,
    Dead,
    Alive
};

class Snake {
private:
    int numPlayers;    // Le nombre de joueurs
    int numReal; // Le nombre de joeurs réels
    int numIAS;     // Le nombre d'IAS
    int tempoMove = 500; // Vitesse du jeu
    int tempoAnimation = 200; // Vitesse du jeu
    long counterAnimation = millis();
    std::vector<Coord> playerCoords; // Coordonnées des joueurs
    std::vector<std::vector<Coord>> playerCoordHistory; // Historique des coordonnées pour chaque joueur
    std::vector<int> playerLength; // Longueur du snake
    std::vector<CRGB> playerColor; // Couleur des joueurs
    std::vector<long> playerMove;  // Les timer des joueurs pour chute de piece
    std::vector<long> snakeMove;  // Les timer des joueurs pour mouvement
    std::vector<Direction> playerDirection; // Direction des joueurs
    std::vector<Board> snakeBoard; // Matrix affichée
    std::vector<Coord> snakeFruits; // MAtrix de fruits
    std::vector<CRGB> colorList;  // List des couleurs
    std::vector<bool> fruitShouldBlink; // Animation des fruits

public:
    Snake(int numP, int numF) : numReal(numP),numIAS(numF) {
        numPlayers = numReal + numIAS;
        colorList = {CRGB(255, 0, 0), CRGB(0, 255, 0), CRGB(0, 0, 255), CRGB(255, 255, 0), CRGB(255, 0, 255)};
        snakeBoard.resize(1,Board(32, 32, emptyColor));
        snakeFruits.resize(numPlayers, Coord(0, 0));
        fruitShouldBlink.resize(numPlayers, false);
        playerCoords.resize(numPlayers, Coord(0, 0));
        playerColor.resize(numPlayers);
        playerDirection.resize(numPlayers);
        playerCoordHistory.resize(numPlayers);
        playerLength.resize(numPlayers, 1);
        snakeMove.resize(numPlayers, millis());
        playerMove.resize(numPlayers, millis());
        snakeBoard[0].position(0, 0);    // Position de la board sur la matrice

        // On trace la déco - Bordure du tableau
        for (int i = 0; i < snakeBoard[0].gameHeight; i++) {
            if (( i > 5 && i < 9) || ( i > 22 && i < 26)) continue;
            snakeBoard[0].board[0][i] = SnakeBorder;
            snakeBoard[0].board[31][i] = SnakeBorder;
            snakeBoard[0].board[i][0] = SnakeBorder;
            snakeBoard[0].board[i][31] = SnakeBorder;
            }

        // On positionne les joueurs
        for (int i = 0; i < numPlayers; i++) {
            if (i == 0)  {
                playerCoords[i] = Coord(7, 7);
                playerDirection[i] = Direction::Right;
            }
            else if (i == 1) {
                playerCoords[i] = Coord(24, 24);
                playerDirection[i] = Direction::Left;
            }
            else if (i == 2) {
                playerCoords[i] = Coord(24,7);
                playerDirection[i] = Direction::Down;
            }
            else if (i == 3) {
                playerCoords[i] = Coord(7,24);
                playerDirection[i] = Direction::Up;
            }
            playerCoordHistory[i].push_back(playerCoords[i]); // On stocke la coordonnée du snake
            playerColor[i] = colorList[i];
            matrix->drawPixel(playerCoords[i].x, playerCoords[i].y, playerColor[i]);
        }

        // On genere les fruits et on les pose
        for (int i = 0; i < numPlayers; i++) {
            snakeFruits[i] = GetRandomCoord(snakeBoard[0].board);
            matrix->drawPixel(snakeFruits[i].x, snakeFruits[i].y, GetRandomColor());
        }        

        snakeBoard[0].show();
        matrix->show();

    }    
  
  void setup() {
    randomSeed(analogRead(0));
  }

  Coord GetRandomCoord(const std::vector<std::vector<CRGB>>& gameBoard) {
    int numRows = gameBoard.size();
    int numCols = (numRows > 0) ? gameBoard[0].size() : 0;
    Coord availableCoord = Coord(1, 1);
    do {
        int x = random(1, numCols - 1);
        int y = random(1, numRows - 1);
        if (gameBoard[y][x] == emptyColor) {
            availableCoord = Coord(x, y);
            break;
        }
    } while (true);
    return availableCoord;
    }

boolean loop() {
    int joueursEnVie = 0;
    for (int i = 0; i < playerDirection.size(); i++) {
        if (playerDirection[i] != Direction::Dead) {
            joueursEnVie++;
        }
    }
    if ( joueursEnVie == 0 ){
        matrix->clear();
        return false;
    }
    // On capture les inputs des joueurs
    for (int i = 0; i < numPlayers; i++) {
        // Tempo déplacement snake
        if ( millis() - snakeMove[i] < tempoMove ) continue;
        snakeMove[i] = millis();
        // L'IA joue
        if ( i >= numPlayers - numIAS ){
            if ( playerDirection[i] != Direction::Dead ) playerDirection[i] = moveAIPlayer(i);
        }
        if (webController.gotMessages(i)){
            String command = webController.getMessages(i);
            if ( command == "left") playerDirection[i] = Direction::Left;
            else if ( command == "right") playerDirection[i] = Direction::Right;
            else if ( command == "down")  playerDirection[i] = Direction::Down;
            else if ( command == "up")  playerDirection[i] = Direction::Up;
        }

        // Direction du serpent
        if ( playerDirection[i] == Direction::Right && playerDirection[i] != Direction::Left) {
            playerCoords[i].x++;
            if ( playerCoords[i].x > 31) playerCoords[i].x = 0;
        }
        else if ( playerDirection[i] == Direction::Left && playerDirection[i] != Direction::Right) {
            playerCoords[i].x--;
            if ( playerCoords[i].x < 0) playerCoords[i].x = 31;
        }
        else if ( playerDirection[i] == Direction::Up && playerDirection[i] != Direction::Down) {
            playerCoords[i].y--;
            if ( playerCoords[i].y < 0 ) playerCoords[i].y = 31;
        }
        else if ( playerDirection[i] == Direction::Down && playerDirection[i] != Direction::Up) {
            playerCoords[i].y++;
            if ( playerCoords[i].y > 31) playerCoords[i].y = 0;
        }

        // Collision avec un élément
        CRGB currentCollisionColor = snakeBoard[0].board[playerCoords[i].x][playerCoords[i].y];
        if ( currentCollisionColor != emptyColor ){
            // Bordure - le joueur est CUIT
            playerDirection[i] = Direction::Dead;
        } else {
            // Pas de collision on store coord du joueur
            snakeBoard[0].board[playerCoords[i].x][playerCoords[i].y] = playerColor[i];
            playerCoordHistory[i].push_back(playerCoords[i]);      
        }
        // Collision avec un fruit ?
        for (int j = 0; j < numPlayers; j++) {
            if ( playerCoords[i].x == snakeFruits[j].x && playerCoords[i].y == snakeFruits[j].y ){
                // On se rallonge
                playerLength[i]+=1;
                // On mange notre propre fruit - rebonus
                // if (i == j){
                //     playerLength[i]+=1;
                // } else {
                //     playerLength[j]-=1
                // }
                // On tire un autre fruit
                snakeFruits[j] = GetRandomCoord(snakeBoard[0].board);
            } 
        }

        // Supression de l'historique des points - et drop du pixel
        while (playerCoordHistory[i].size() > playerLength[i]) {
            Coord oldCoord = playerCoordHistory[i].front();
            snakeBoard[0].board[oldCoord.x][oldCoord.y] = emptyColor;
            matrix->drawPixel(oldCoord.x, oldCoord.y, emptyColor);
            playerCoordHistory[i].erase(playerCoordHistory[i].begin());
        }
        snakeBoard[0].show();
        
    }
    // On anime les fruits
    if (millis() - counterAnimation >= tempoAnimation) {
        counterAnimation = millis();
        for (int j = 0; j < numPlayers; j++) {
            if (fruitShouldBlink[j]) {
                matrix->drawPixel(snakeFruits[j].x, snakeFruits[j].y, colorList[j]);
                fruitShouldBlink[j] = false;
            } else {
                matrix->drawPixel(snakeFruits[j].x, snakeFruits[j].y, CRGB(255, 255, 255));
                fruitShouldBlink[j] = true;
            }
        }
    }

    matrix->show();    
    return true; // Pour continuer le loop
    }

    Direction moveAIPlayer(int playerID) {
        int x = playerCoords[playerID].x;
        int y = playerCoords[playerID].y;

        Direction currentDirection = playerDirection[playerID];  // Conservez la direction actuelle
        // Générer la liste des directions possibles
        std::vector<Direction> possibleDirections;

        if (y < 31 && currentDirection != Direction::Up && snakeBoard[0].board[x][y + 1] == emptyColor) {
            possibleDirections.push_back(Direction::Down);
        }
        if (x < 31 && currentDirection != Direction::Left && snakeBoard[0].board[x + 1][y] == emptyColor) {
            possibleDirections.push_back(Direction::Right);
        }
        if (y > 0 && currentDirection != Direction::Down && snakeBoard[0].board[x][y - 1] == emptyColor) {
            possibleDirections.push_back(Direction::Up);
        }
        if (x > 0 && currentDirection != Direction::Right && snakeBoard[0].board[x - 1][y] == emptyColor) {
            possibleDirections.push_back(Direction::Left);
        }

        // Vérifiez si le serpent est sur le point de se déplacer vers une bordure (portail)
        if (x == 0 && currentDirection != Direction::Right) {
            possibleDirections.push_back(Direction::Left);
        }
        if (x == 31 && currentDirection != Direction::Left) {
            possibleDirections.push_back(Direction::Right);
        }
        if (y == 0 && currentDirection != Direction::Down) {
            possibleDirections.push_back(Direction::Up);
        }
        if (y == 31 && currentDirection != Direction::Up) {
            possibleDirections.push_back(Direction::Down);
        }

        // Recherchez un fruit parmi les directions possibles
        Coord nearestFruit = FindNearestFruit(playerCoords[playerID], snakeFruits);
        if (nearestFruit.x > x && std::find(possibleDirections.begin(), possibleDirections.end(), Direction::Right) != possibleDirections.end()) {
            return Direction::Right;
        }
        if (nearestFruit.x < x && std::find(possibleDirections.begin(), possibleDirections.end(), Direction::Left) != possibleDirections.end()) {
            return Direction::Left;
        }
        if (nearestFruit.y > y && std::find(possibleDirections.begin(), possibleDirections.end(), Direction::Down) != possibleDirections.end()) {
            return Direction::Down;
        }
        if (nearestFruit.y < y && std::find(possibleDirections.begin(), possibleDirections.end(), Direction::Up) != possibleDirections.end()) {
            return Direction::Up;
        }       

        // // Si aucun fruit n'est trouvé dans les directions possibles, conserver la direction actuelle si possible
        // if (std::find(possibleDirections.begin(), possibleDirections.end(), currentDirection) != possibleDirections.end()) {
        //     return currentDirection;
        // }

        // Selection random de la direction
        if (!possibleDirections.empty()) {
            // //int randomIndex = std::rand() % possibleDirections.size(); // Génère un indice aléatoire
            int randomIndex = random(0, possibleDirections.size() - 1);
            return possibleDirections[randomIndex]; // Renvoie l'élément correspondant à cet indice
        }
        return currentDirection;        
    }

    Coord FindNearestFruit(Coord currentPos, const std::vector<Coord>& fruits) {
        Coord nearestFruit = fruits[0];
        int minDistance = CalculateDistance(currentPos, nearestFruit);

        for (const Coord& fruit : fruits) {
            int distance = CalculateDistance(currentPos, fruit);
            if (distance < minDistance) {
                nearestFruit = fruit;
                minDistance = distance;
            }
        }
        return nearestFruit;
    }

    int CalculateDistance(Coord pointA, Coord pointB) {
        return abs(pointA.x - pointB.x) + abs(pointA.y - pointB.y);
    }

};

