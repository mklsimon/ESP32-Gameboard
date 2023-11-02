#include <FastLED_NeoMatrix.h>
#include "config.h"
#include <vector>
#include <map>
#include "Tiny3x3a2pt7b.h"
#include "gameBoardlib-WebControler.h"
#include "gameBoardlib-Board.h"
#include "gameBoardlib-Menu.h"

// #include <BluetoothSerial.h>
// BluetoothSerial SerialBT;

WebControler webController;
MatrixMenu menu;

// Games
#include "game-tetris.h"
#include "game-snake.h"

CRGB matrixleds[MATRIX_SIZE];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_HEIGHT, MATRIX_WIDTH, MATRIX_TILE_H, MATRIX_TILE_V, MATRIX_TYPE);

void setup() {
  // DEBUG
  Serial.begin(115200);
  while (!Serial);
  FastLED.addLeds<NEOPIXEL,LED_PIN>(matrixleds, MATRIX_SIZE); 
  matrix->begin();
  matrix->setFont( &Tiny3x3a2pt7b);
  matrix->setTextSize(1);
  matrix->setTextWrap(false);
  matrix->setBrightness(5);
  matrix->setTextColor(CRGB::Blue);
  //SerialBT.begin("espMatrix");
  webController.begin();
  initMenu();
}

void loop() {
  String result = menu.show("JEUX");
  if (result == "TETRIS") runTetris();
  if (result == "SNAKE") runSnake();
  delay(1000);
  initMenu();
}


void runSnake(){
  menu.clear();
  menu.add_item("1");
  menu.add_item("2");
  menu.add_item("3");
  String joueurs = menu.show("Joueurs ?");
  menu.clear();
  menu.add_item("1");
  menu.add_item("2");
  menu.add_item("3");
  menu.add_item("4");
  String fruits = menu.show("fruits ?");
  bool isRunning = true;
  Snake snake = Snake(joueurs.toInt(),fruits.toInt(),1);
  snake.setup();
  while (isRunning) {
    isRunning = snake.loop();
  }
  initMenu();
}

void runTetris(){
  menu.clear();
  menu.add_item("1");
  menu.add_item("2");
  menu.add_item("3");
  String result = menu.show("Joueurs ?");
  bool isRunning = true;
  Tetris tetris = Tetris(result.toInt());
  tetris.setup();
  while (isRunning) {
    isRunning = tetris.loop();
  }
  initMenu();
}
void initMenu(){
  menu.clear();
  std::vector<std::vector<CRGB>> iconTetris = {
      {CRGB(0, 0, 0), CRGB(0, 255, 0), CRGB(0, 0, 0)},
      {CRGB(0, 0, 0), CRGB(0, 255, 0), CRGB(0, 255, 0)},
      {CRGB(0, 0, 0), CRGB(0, 255, 0), CRGB(0, 0, 0)}
  };
  menu.add_item("TETRIS", iconTetris);
  menu.add_item("SNAKE", iconTetris);
  menu.add_item("PACMAN", iconTetris);
}