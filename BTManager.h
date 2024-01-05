// BTManager.h
#pragma once
#include <Bluepad32.h>

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
      ControllerPtr myController = myControllers[i];

      if (myController && myController->isConnected()) {
        if (myController->isGamepad())
          processGamepad(myController);
      }
    }
  }

private:
  ControllerPtr myControllers[BP32_MAX_CONTROLLERS];

  void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
      if (myControllers[i] == nullptr) {
        Serial.print("CALLBACK: Controller is connected, index=");
        Serial.println(i);
        myControllers[i] = ctl;
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
          if (myControllers[i] == ctl) {
              Serial.print("CALLBACK: Controller is disconnected from index=");
              Serial.println(i);
              myControllers[i] = nullptr;
              break;
          }
      }
  }

  void processGamepad(ControllerPtr gamepad) {
    Serial.printf("idx=%d, axis L: %4d %4d, axis R: %4d %4d, buttons %4d, dpad L: %4d, brake: %4d, throttle: %4d, miscButtons %4d\n", 
      gamepad->index(), gamepad->axisX(), gamepad->axisY(), gamepad->axisRX(), gamepad->axisRY(), gamepad->buttons(), gamepad->dpad(), gamepad->brake(), gamepad->throttle(), gamepad->miscButtons());
    }
};
