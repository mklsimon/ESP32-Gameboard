// WS2812 infos
#define LED_PIN 13
#define MATRIX_WIDTH        16 // largeur de CHAQUE MATRICE NEOPIXEL
#define MATRIX_HEIGHT       16 // hauteur de chaque matrice
#define MATRIX_TILE_H       3  // nombre de matrices disposées horizontalement
#define MATRIX_TILE_V       3  // nombre de matrices disposées verticalement
#define MATRIX_SIZE        ( MATRIX_WIDTH * MATRIX_HEIGHT * MATRIX_TILE_H * MATRIX_TILE_V )
#define MATRIX_TYPE        ( NEO_MATRIX_ZIGZAG + NEO_TILE_ZIGZAG + NEO_MATRIX_LEFT + NEO_MATRIX_BOTTOM )
// Flag de disposition de la matrice, ajoutez-les au besoin
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the FIRST MATRIX; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs WITHIN EACH MATRIX are
//     arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns WITHIN
//     EACH MATRIX proceed in the same order, or alternate lines reverse
//     direction; pick one.
//   NEO_TILE_TOP, NEO_TILE_BOTTOM, NEO_TILE_LEFT, NEO_TILE_RIGHT:
//     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
//     two, e.g. NEO_TILE_TOP + NEO_TILE_LEFT for the top-left corner.
//   NEO_TILE_ROWS, NEO_TILE_COLUMNS: the matrices in the OVERALL DISPLAY
//     are arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_TILE_PROGRESSIVE, NEO_TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
//     (tiles) in the OVERALL DISPLAY proceed in the same order for every
//     line, or alternate lines reverse direction; pick one.  When using
//     zig-zag order, the orientation of the matrices in alternate rows
//     will be rotated 180 degrees (this is normal -- simplifies wiring).

const CRGB emptyColor = CRGB::Black;
const CRGB whiteColor = CRGB::White;
const CRGB borderColor = CRGB(255, 255, 255);
const CRGB tetrisColor1 = CRGB(255, 0, 0);    // Rouge
const CRGB tetrisColor2 = CRGB(0, 255, 0);    // Vert
const CRGB tetrisColor3 = CRGB(0, 128, 255);  // Bleu
const CRGB tetrisColor4 = CRGB(255, 255, 0);  // Jaune
const CRGB tetrisColor5 = CRGB(255, 0, 255);  // Magenta
const CRGB tetrisColor6 = CRGB(0, 255, 255);  // Cyan
const CRGB tetrisColor7 = CRGB(0, 128, 255);

const CRGB titleMenuColor = CRGB(255, 255, 0);
const CRGB borderMenuColor = CRGB(0, 0, 255);
const CRGB TextMenuColor = CRGB(255, 255, 255);

const CRGB SnakeBorder = CRGB(255, 255, 255);