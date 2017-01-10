#include "MeOrion.h"
#include <SoftwareSerial.h>

MeDCMotor cingoloA(M2);
MeDCMotor cingoloB(M1);
MeRGBLed led(PORT_6);

int powerMotoreA = 0;
int powerMotoreB = 0;
int numeroMorto = 0;

String readBuf;

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    readBuf = Serial.readStringUntil('\n');
    int posComma = readBuf.indexOf(',');
    int posSemicolon = readBuf.indexOf(';');

    powerMotoreA = readBuf.substring(0, posComma).toInt();
    powerMotoreB = readBuf.substring(posComma + 1, posSemicolon).toInt();
    numeroMorto = readBuf.substring(posSemicolon + 1).toInt();

    speedMotor(cingoloA, powerMotoreA);
    speedMotor(cingoloB, powerMotoreB);
    Serial.println(powerMotoreA) && Serial.println(powerMotoreB);
  }
}

bool morto() {
  return true;
}

void speedMotor(MeDCMotor motore, int speed) {
  motore.run(speed);
}

void ledColors(int nLed, int r, int g, int b) {
  for (int i = 0; i < nLed; i++)
    led.setColorAt(i, r, g, b);

  led.show();
}
