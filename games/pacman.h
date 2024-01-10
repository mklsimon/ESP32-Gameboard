extern FastLED_NeoMatrix *matrix; // Déclaration de la variable globale

const CRGB pacmanBorder = CRGB(0, 0, 255);

std::vector<std::vector<CRGB>> pacmanPixel = {
  {emptyColor, tetrisColor4, tetrisColor4},
  {tetrisColor2, tetrisColor2, emptyColor},
  {emptyColor, tetrisColor4, tetrisColor4}
};

class Pacman {
    private:
        std::vector<Board> pacmanBoard; //Espace de jeux
        std::vector<Coord> pacmanCoords;
        std::vector<Coord> monsterCoords;
        int lap = 0;
        int numPlayers;

    public:
    Pacman(int numP = 1) : numPlayers(numP){
        monsterCoords.resize(numPlayers, Coord(0, 0));
        pacmanCoords.resize(numPlayers, Coord(0, 0));

        pacmanBoard.resize(1,Board(50, 50, emptyColor,31,31)); 
        pacmanBoard[0].position(0, 0);
        // On trace la déco - Bordure du tableau
        for (int i = 0; i < pacmanBoard[0].gameHeight; i++) {
            pacmanBoard[0].board[0][i] = pacmanBorder;
            pacmanBoard[0].board[pacmanBoard[0].gameHeight - 1][i] = pacmanBorder;
            pacmanBoard[0].board[i][0] = pacmanBorder;
            pacmanBoard[0].board[i][pacmanBoard[0].gameWidth - 1] = pacmanBorder;
            }
        
        for (int i = 0; i < 10; i++) {
            int x = random(50);  // Génère un nombre aléatoire entre 0 et 100
            int y = random(50);  // Génère un nombre aléatoire entre 0 et 100
            pacmanBoard[0].board[x][y] = CRGB(0, 255, 0);
            pacmanBoard[0].board[y][x] = CRGB(255, 0, 0);
            }
        }        
    
    void setup() {
            randomSeed(analogRead(0));
        }

    // Boucle de la class
    boolean loop() {
        delay(100);
        pacmanBoard[0].show(false,lap,lap);
        matrix->show();    
        lap+=1;
        return true; // Pour continuer le loop
        }

};