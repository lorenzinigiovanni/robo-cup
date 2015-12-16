#include "MeOrion.h"
#include <SoftwareSerial.h>

MeBluetooth bluetooth(PORT_5);

MeDCMotor Pinza(PORT_1);
MeDCMotor Braccio(PORT_2);
MeDCMotor cingoloA(M1);
MeDCMotor cingoloB(M2);
MeRGBLed led(PORT_6);

MeUltrasonicSensor ultraSensor(PORT_7);

void setup() {
  bluetooth.begin(115200);
  ledColors(255, 0, 255);
}

void loop() {
  speedMotor(cingoloA, -distanza());
  speedMotor(cingoloB, distanza());
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


