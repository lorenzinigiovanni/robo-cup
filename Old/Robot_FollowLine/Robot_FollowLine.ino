#include "MeOrion.h"
#include <SoftwareSerial.h>

MeBluetooth bluetooth(PORT_5);

MeDCMotor cingoloA(M1);
MeDCMotor cingoloB(M2);
MeRGBLed led(PORT_6);

MeUltrasonicSensor ultraSensor(PORT_7);
MeLineFollower lineFinder(PORT_4);

void setup() {
  bluetooth.begin(115200);
  ledColors(255, 0, 255);
}

void loop() {
  /*speedMotor(cingoloA, -distanza());
    speedMotor(cingoloB, distanza());*/

  int sensorState = lineFinder.readSensors();
  switch (sensorState)
  {
    case S1_IN_S2_IN:
      speedMotor(cingoloA, 30);
      speedMotor(cingoloB, -30);
      break;
    case S1_IN_S2_OUT:
      speedMotor(cingoloA, 30);
      speedMotor(cingoloB, 40);
      break;
    case S1_OUT_S2_IN:
      speedMotor(cingoloA, -40);
      speedMotor(cingoloB, -30);
      break;
    case S1_OUT_S2_OUT:
      speedMotor(cingoloA, 0);
      speedMotor(cingoloB, 0);
      break;
    default: break;
  }
}

void speedMotor(MeDCMotor motore, int speed) {
  motore.run(speed);
}

float distanza() {
  return ultraSensor.distanceCm();
}

void ledColors(int r, int g, int b) {
  for (int i = 0; i < 15; i++)
    led.setColorAt(i, r, g, b);

  led.show();
}


