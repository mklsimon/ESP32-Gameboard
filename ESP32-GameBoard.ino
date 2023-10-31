// #include <Adafruit_GFX.h>
// #include <FastLED.h>
#include <vector>
#include <FastLED_NeoMatrix.h>

#include "config.h"
#include "lib-gameBoard.h"
#include "lib-gameWebControler.h"

// #include <BluetoothSerial.h>
// BluetoothSerial SerialBT;

WebControler webController;

// Games
#include "game-tetris.h"

CRGB matrixleds[MATRIX_SIZE];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_HEIGHT, MATRIX_WIDTH, MATRIX_TILE_H, MATRIX_TILE_V, MATRIX_TYPE);


void setup() {
  // DEBUG
  Serial.begin(115200);
  while (!Serial);
  FastLED.addLeds<NEOPIXEL,LED_PIN>(matrixleds, MATRIX_SIZE); 
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(5);
  matrix->setTextColor(CRGB::Blue);
  //SerialBT.begin("espMatrix");
  webController.begin();
}

void loop() {
  runTetris();
}

void runTetris(){
  bool isRunning = true;
  Tetris tetris = Tetris(2);
  tetris.setup();
  while (isRunning) {
    isRunning = tetris.loop();
  }
}