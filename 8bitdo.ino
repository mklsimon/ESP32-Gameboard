#include "BTManager.h"
BluetoothControllerManager BTcontroler;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  BTcontroler.initialize();
}

void loop() {
  BTcontroler.updateControllers();
  delay(150);  // Gardez une petite pause pour ne pas surcharger la boucle
}
