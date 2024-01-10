#include <FastLED_NeoMatrix.h>
#include <vector>
#include <map>
#include "configs/config-matrix.h"
#include "fonts/Tiny3x3a2pt7b.h"

// Bluetooth Manager
#include "libs/BTManager.h"
BluetoothControllerManager BT;

// #include <vector>
// #include <map>
// #include "Tiny3x3a2pt7b.h"
// #include "gameBoardlib-WebControler.h"
#include "libs/gameBoard.h"
#include "libs/gameMenu.h"

// // #include <BluetoothSerial.h>
// // BluetoothSerial SerialBT;

// WebControler webController;
MatrixMenu menu;

// // Games
#include "games/tetris.h"
// #include "games/snake.h"
#include "games/pacman.h"

CRGB matrixleds[MATRIX_SIZE];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_HEIGHT, MATRIX_WIDTH, MATRIX_TILE_H, MATRIX_TILE_V, MATRIX_TYPE);

void setup() {
  // Serial
  Serial.begin(115200);
  while (!Serial);

  // Init Matrix
  FastLED.addLeds<NEOPIXEL,LED_PIN>(matrixleds, MATRIX_SIZE); 
  matrix->begin();
  //matrix->setFont(&Tiny3x3a2pt7b);
  matrix->setTextSize(1);
  matrix->setTextWrap(false);
  matrix->setBrightness(5);
  matrix->clear();
  matrix->show();

  // Init BT
  BT.initialize();
  // //webController.begin();
  // initMenu();
}

void loop() {  
  // runPacman();
  // menu.clear();
  // menu.add_item("1");
  // menu.add_item("2");
  // menu.add_item("3");
  // String result = menu.ask("Joueurs ?");
  Serial.println("loop");
  bool isRunning = true;
  Tetris tetris = Tetris(3);
  tetris.setup();
  while (isRunning) {
    isRunning = tetris.loop();
  }

  // runPacman();
  // //runDemo();
  // // // runPacman();
  
  // String result = menu.show("JEUX");
  // if (result == "TETRIS") runTetris();
  // if (result == "SNAKE") runSnake();
  // delay(1000);
  // initMenu();
}

void showPadInfo(){
  ControllerInfo pad = BT.getControllerStatus(0);
  Serial.printf("ONLINE: %d, UP: %d, DOWN: %d, LEFT: %d, RIGHT: %d, START: %d, SELECT: %d, SQUARE: %d, CIRCLE: %d, CROSS: %d, TRIANGLE: %d, PS: %d\n",
              pad.ONLINE, pad.UP, pad.DOWN, pad.LEFT, pad.RIGHT, pad.START, pad.SELECT, pad.SQUARE, pad.CIRCLE, pad.CROSS, pad.TRIANGLE, pad.PS3);

  delay(150);  // Gardez une petite pause pour ne pas surcharger la boucle
}

void runPacman(){
  bool isRunning = true;
  Pacman pacman = Pacman();
  pacman.setup();
  while (isRunning) {
    isRunning = pacman.loop();
  }
  //initMenu();
}

// void runSnake(){
//   menu.clear();
//   menu.add_item("0");
//   menu.add_item("1");
//   menu.add_item("2");
//   menu.add_item("3");
//   String joueurs = menu.show("Joueurs ?");
//   menu.clear();
//   if ( joueurs.toInt() > 2 ) menu.add_item("1");
//   if ( joueurs.toInt() > 1 ) menu.add_item("2");
//   if ( joueurs.toInt() > 0 ) menu.add_item("3");
//   if ( joueurs.toInt() == 0 ) menu.add_item("4");
//   String fruits = menu.show("IA ?");
//   bool isRunning = true;
//   Snake snake = Snake(joueurs.toInt(),fruits.toInt());
//   snake.setup();
//   while (isRunning) {
//     isRunning = snake.loop();
//   }
//   initMenu();
// }

void runTetris(){
  menu.clear();
  menu.add_item("1");
  menu.add_item("2");
  menu.add_item("3");
  String result = menu.ask("Joueurs ?");
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