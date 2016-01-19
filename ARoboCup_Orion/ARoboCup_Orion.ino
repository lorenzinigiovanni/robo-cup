#include "MeOrion.h"
#include <SoftwareSerial.h>

MeDCMotor cingoloA(M1);
MeDCMotor cingoloB(M2);
MeRGBLed led(PORT_6);

int powerMotoreA = 0;
int powerMotoreB = 0;
int numeroMorto = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {

    powerMotoreA = Serial.parseInt();
    powerMotoreB = Serial.parseInt();
    numeroMorto = Serial.parseInt();

    speedMotor(cingoloA, powerMotoreA);
    speedMotor(cingoloB, powerMotoreB);
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
