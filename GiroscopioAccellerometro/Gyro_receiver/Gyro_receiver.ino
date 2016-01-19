#include "MeOrion.h"
#include <SoftwareSerial.h>

MeDCMotor cingoloA(M1);
MeDCMotor cingoloB(M2);
MeRGBLed led(PORT_6);
MeUltrasonicSensor ultraSensor(PORT_7);

String readBuf;

float ypr[3] = {0.0, 0.0, 0.0};
float currentYaw = 0;
float setYaw = 0;
float c = 0.0;

int forwardSpeed = 60;
int turnSpeed = 50;
int direction = 0;
void setup() {
  Serial.begin(115200);
  //  ledColors(15, 255, 0, 255);
  delay(5000);
  direction = gyroscope(0);
}

void loop() {
//  keepLine(direction);

  /*if (distanza() <= 10.0) {
    c = gyroNav();
    while (gyroscope(0) - c <= 0) {
      speedMotor(cingoloA, -turnSpeed);
      speedMotor(cingoloB, turnSpeed);
    }
    direction = gyroscope(0);
    }
    else {
    keepLine(direction);
    }*/
  /*
    while (gyroscope(2) >= 20) {
      speedMotor(cingoloA, 0);
      speedMotor(cingoloB, 0);
    }*/
    Serial.print("Yaw: ");
    Serial.print(gyroscope(0));
    Serial.print("\t");
    Serial.print("Set Yaw: ");
    Serial.print(gyroNav());

    Serial.print("\tPitch: ");
    Serial.print(gyroscope(1));

    Serial.print("\tRoll: ");
    Serial.println(gyroscope(2));
}

float gyroscope(int sel) {
  if (Serial.available()) {
    readBuf = Serial.readStringUntil('\n');
    int posComma = readBuf.indexOf(',');
    int posSemicolon = readBuf.indexOf(';');

    ypr[0] = readBuf.substring(0, posComma - 1).toFloat();
    ypr[1] = readBuf.substring(posComma + 1, posSemicolon - 1).toFloat();
    ypr[2] = readBuf.substring(posSemicolon + 1).toFloat();
  }

  return ypr[sel];
}

float gyroNav() {
  currentYaw = gyroscope(0);
  setYaw = currentYaw + 90.0;

  if (setYaw > 360.0) {
    float diff = setYaw - 360.0;
    return setYaw = diff;
  }
  else
    return setYaw;
}

void keepLine(int dir) {
  if (dir > gyroscope(0))
    gyroTurnLeft();
  else if (dir < gyroscope(0))
    gyroTurnRight();
  else
    goForward(forwardSpeed);
}

void goForward(int speed) {
  speedMotor(cingoloA, speed);
  speedMotor(cingoloB, speed);
}

void gyroTurnLeft() {
  speedMotor(cingoloA, forwardSpeed);
  speedMotor(cingoloB, forwardSpeed - 30);
}

void gyroTurnRight() {
  speedMotor(cingoloA, forwardSpeed - 30);
  speedMotor(cingoloB, forwardSpeed);
}

void turnLeft(int speed) {
  speedMotor(cingoloA, speed);
  speedMotor(cingoloB, -speed);
}

void turnRight(int speed) {
  speedMotor(cingoloA, -speed);
  speedMotor(cingoloB, speed);
}

void speedMotor(MeDCMotor motore, int speed) {
  motore.run(speed);
}

float distanza() {
  return ultraSensor.distanceCm();
}

/*
  bool move(int type, float distance, int direction, int speed) {
  switch (type) {
    case 0:

      break;
  }
  return true;
  }*/

void ledColors(int nLed, int r, int g, int b) {
  for (int i = 0; i < nLed; i++)
    led.setColorAt(i, r, g, b);

  led.show();
}


