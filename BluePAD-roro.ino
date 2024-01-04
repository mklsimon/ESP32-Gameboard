// Copyright 2021 - 2023, Ricardo Quesada
// SPDX-License-Identifier: Apache 2.0 or LGPL-2.1-or-later

/*
 * This example shows how to use the Controller API.
 *
 * Supported on boards with NINA W10x. In particular these boards:
 *  - Arduino MKR WiFi 1010,
 *  - UNO WiFi Rev.2,
 *  - Nano RP2040 Connect,
 *  - Nano 33 IoT,
 *  - Arduino Arduino MKR Vidor 4000
 */
#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_CONTROLLERS];

// Arduino setup function. Runs in CPU 1
void setup() {
  // Initialize serial
  Serial.begin(115200);
  while (!Serial) {
    // wait for serial port to connect.
    // You don't have to do this in your game. This is only for debugging
    // purposes, so that you can see the output in the serial console.
    ;
  }

  String fv = BP32.firmwareVersion();
  Serial.print("Firmware version installed: ");
  Serial.println(fv);

  // To get the BD Address (MAC address) call:
  const uint8_t* addr = BP32.localBdAddress();
  Serial.print("BD Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(addr[i], HEX);
    if (i < 5)
      Serial.print(":");
    else
      Serial.println();
  }

  // BP32.pinMode(27, OUTPUT);
  // BP32.digitalWrite(27, 0);

  // This call is mandatory. It setups Bluepad32 and creates the callbacks.
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();
}

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.print("CALLBACK: Controller is connected, index=");
      Serial.println(i);
      myControllers[i] = ctl;
      foundEmptySlot = true;

      // Optional, once the gamepad is connected, request further info about the
      // gamepad.
      ControllerProperties properties = ctl->getProperties();
      char buf[80];
      sprintf(buf,
              "BTAddr: %02x:%02x:%02x:%02x:%02x:%02x, VID/PID: %04x:%04x, "
              "flags: 0x%02x",
              properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
              properties.btaddr[3], properties.btaddr[4], properties.btaddr[5],
              properties.vendor_id, properties.product_id, properties.flags);
      Serial.println(buf);
      break;
    }
  }
  if (!foundEmptySlot) {setup
    Serial.println(
        "CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.print("CALLBACK: Controller is disconnected from index=");
      Serial.println(i);
      myControllers[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad) {
    Serial.println(
        "CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

void processGamepad(ControllerPtr gamepad) {
  // There are different ways to query whether a button is pressed.
  // By query each button individually:
  //  a(), b(), x(), y(), l1(), etc...

  if (gamepad->a()) {
    static int colorIdx = 0;
    // Some gamepads like DS4 and DualSense support changing the color LED.
    // It is possible to change it by calling:
    switch (colorIdx % 3) {
      case 0:
        // Redsetup
        gamepad->setColorLED(255, 0, 0);
        break;
      case 1:
        // Green
        gamepad->setColorLED(0, 255, 0);
        break;
      case 2:
        // Blue
        gamepad->setColorLED(0, 0, 255);
        break;
    }
    colorIdx++;
  }

  if (gamepad->b()) {
    // Turn on the 4 LED. Each bit represents one LED.
    static int led = 0;
    led++;
    // Some gamepads like the DS3, DualSense, Nintendo Wii, Nintendo Switch
    // support changing the "Player LEDs": those 4 LEDs that usually indicate
    // the "gamepad seat".
    // It is possible to change them by calling:
    gamepad->setPlayerLEDs(led & 0x0f);
  }

  if (gamepad->x()) {
    // Duration: 255 is ~2 seconds
    // force: intensity
    // Some gamepads like DS3, DS4, DualSense, Switch, Xbox One S support
    // rumble.
    // It is possible to set it by calling:
    gamepad->setRumble(0xc0 /* force */, 0xc0 /* duration */);
  }
  dumpGamepad(gamepad);
}

void dumpGamepad(ControllerPtr gamepad) {
  if (gamepad->axisY() > 200) {
      Serial.printf("Down         ! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisX(), gamepad->axisY(), gamepad->buttons(), gamepad->battery());
    } else if (gamepad->axisY() < -200) {
      Serial.printf("Up           ! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisX(), gamepad->axisY(), gamepad->buttons(), gamepad->battery());
    } else if (gamepad->axisX() < -50) {
      Serial.printf("Rotate left  ! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisX(), gamepad->axisY(), gamepad->buttons(), gamepad->battery());
    } else if (gamepad->axisX() > 50) {
      Serial.printf("Rotate right ! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisX(), gamepad->axisY(), gamepad->buttons(), gamepad->battery());
    }
    if (gamepad->axisRY() > 200) {
      Serial.printf("Down         ! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisRX(), gamepad->axisRY(), gamepad->buttons(), gamepad->battery());
    } else if (gamepad->axisRY() < -200) {
      Serial.printf("Up           ! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisRX(), gamepad->axisRY(), gamepad->buttons(), gamepad->battery());
    } else if (gamepad->axisRX() < -50) {
      Serial.printf("Go left      ! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisRX(), gamepad->axisRY(), gamepad->buttons(), gamepad->battery());
    } else if (gamepad->axisRX() > 50) {
      Serial.printf("Go right     ! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisRX(), gamepad->axisRY(), gamepad->buttons(), gamepad->battery());
    }

    byte button = gamepad->buttons();
    if ((button & 0x0001) == 0x0001) {
      Serial.printf("Cross press  ! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisRX(), gamepad->axisRY(), gamepad->buttons(), gamepad->battery());
    } else if ((button & 0x0002) == 0x0002) {
      Serial.printf("Cirlcle press! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisRX(), gamepad->axisRY(), gamepad->buttons(), gamepad->battery());
    } else if ((button & 0x0004) == 0x0004) {
      Serial.printf("Square press ! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisRX(), gamepad->axisRY(), gamepad->buttons(), gamepad->battery());
    } else if ((button & 0x0008) == 0x0008) {
      Serial.printf("Triang. press! idx=%d, axis L: %4d %4d, buttons %4d, battery %4d\n", gamepad->index(), gamepad->axisRX(), gamepad->axisRY(), gamepad->buttons(), gamepad->battery());
    }
}

// Arduino loop function. Runs in CPU 1
void loop() {
  // This call fetches all the controller info from the NINA (ESP32) module.
  // Just call this function in your main loop.
  // The controllers pointer (the ones received in the callbacks) gets updated
  // automatically.
  BP32.update();

  // It is safe to always do this before using the controller API.
  // This guarantees that the controller is valid and connected.
  for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
    ControllerPtr myController = myControllers[i];

    if (myController && myController->isConnected()) {
      if (myController->isGamepad())
        processGamepad(myController);
    }
  }
  // TODO
  // disable new pad whenever ready:  enableNewBluetoothConnections

  delay(150);
}

