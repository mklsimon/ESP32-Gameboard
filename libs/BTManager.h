// BTManager.h
#pragma once
#include <Bluepad32.h>

int X_THRESHOLD = 200;
int Y_THRESHOLD = 200;

struct ControllerInfo {
  bool ONLINE;
  bool UP;
  bool DOWN;
  bool LEFT;
  bool RIGHT;
  bool START;
  bool SELECT;
  bool PS3;
  bool CROSS;
  bool CIRCLE;
  bool TRIANGLE;
  bool SQUARE;
};

class BluetoothControllerManager {
public:
  BluetoothControllerManager() {
    // Constructeur

  }

  void initialize() {
    Serial.begin(115200);
    while (!Serial) {}

    String fv = BP32.firmwareVersion();
    Serial.print("Firmware version installed: ");
    Serial.println(fv);

    const uint8_t* addr = BP32.localBdAddress();
    Serial.print("BD Address: ");
    for (int i = 0; i < 6; i++) {
      Serial.print(addr[i], HEX);
      if (i < 5)
        Serial.print(":");
      else
        Serial.println();
    }

    BP32.setup(
        [this](ControllerPtr ctl) { onConnectedController(ctl); },
        [this](ControllerPtr ctl) { onDisconnectedController(ctl); }
    );
    BP32.forgetBluetoothKeys();
  }

  void updateControllers() {
    BP32.update();
    for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
      ControllerPtr gamepad = gamepads[i];

      if (gamepad && gamepad->isConnected()) {
        if (gamepad->isGamepad())
          processGamepad(gamepad);
      }
    }
  }
  
  void updateController(int controllerId) {
    BP32.update();
    if (controllerId >= 0 && controllerId < BP32_MAX_CONTROLLERS) {
      ControllerPtr gamepad = gamepads[controllerId];

      if (gamepad && gamepad->isConnected()) {
        if (gamepad->isGamepad())
          processGamepad(gamepad);
      }
    } else {
      Serial.println("Invalid controller ID");
    }
  }

  ControllerInfo getControllerStatus(int controllerId) {
    BP32.update();
    ControllerInfo controllerInfo = {0}; // Initialisez la structure avec des valeurs par défaut

    if (controllerId >= 0 && controllerId < BP32_MAX_CONTROLLERS) {
      ControllerPtr gamepad = gamepads[controllerId];

      if (gamepad && gamepad->isConnected() && gamepad->isGamepad()) {
        controllerInfo.ONLINE = 1;
        // Stick left
        controllerInfo.DOWN = gamepad->axisY() > Y_THRESHOLD ;
        controllerInfo.UP = gamepad->axisY() < -Y_THRESHOLD;
        controllerInfo.RIGHT = gamepad->axisX() > X_THRESHOLD;
        controllerInfo.LEFT = gamepad->axisX() < -X_THRESHOLD;

        // Button
        byte button = gamepad->buttons();
        controllerInfo.CROSS = (button & 0x0001) == 0x0001;
        controllerInfo.CIRCLE = (button & 0x0002) == 0x0002;
        controllerInfo.SQUARE = (button & 0x0004) == 0x0004;
        controllerInfo.TRIANGLE = (button & 0x0008) == 0x0008;

        // Misc
        button = gamepad->miscButtons();
        controllerInfo.PS3 = (button & 0x0001) == 0x0001;
        controllerInfo.SELECT = (button & 0x0002) == 0x0002;
        controllerInfo.START = (button & 0x0004) == 0x0004;
      }
    }
    return controllerInfo;
  }

private:
  ControllerPtr gamepads[BP32_MAX_CONTROLLERS];

  void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
      if (gamepads[i] == nullptr) {
        Serial.print("CALLBACK: Controller is connected, index=");
        Serial.println(i);
        gamepads[i] = ctl;
        foundEmptySlot = true;
        ControllerProperties properties = ctl->getProperties();
        Serial.printf("BTAddr: %02x:%02x:%02x:%02x:%02x:%02x, VID/PID: %04x:%04x, flags: 0x%02x\n",
              properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
              properties.btaddr[3], properties.btaddr[4], properties.btaddr[5],
              properties.vendor_id, properties.product_id, properties.flags);
        break;
      }
    }
  }

  void onDisconnectedController(ControllerPtr ctl) {
      for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
          if (gamepads[i] == ctl) {
              Serial.print("CALLBACK: Controller is disconnected from index=");
              Serial.println(i);
              gamepads[i] = nullptr;
              break;
          }
      }
  }

  void processGamepad(ControllerPtr gamepad) {
    Serial.printf("idx=%d, axis L: %4d %4d, axis R: %4d %4d, buttons %4d, dpad L: %4d, brake: %4d, throttle: %4d, miscButtons %4d\n", 
      gamepad->index(), gamepad->axisX(), gamepad->axisY(), gamepad->axisRX(), gamepad->axisRY(), gamepad->buttons(), gamepad->dpad(), gamepad->brake(), gamepad->throttle(), gamepad->miscButtons());
    }
};
