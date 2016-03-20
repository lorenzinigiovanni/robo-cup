#include <Wire.h>
#include <SoftwareWire.h>
SoftwareWire myWire(2, 3); //(sda, scl)
SoftwareWire myWire1(4, 5); //(sda, scl)

#define clear 0x14
#define red 0x16
#define green 0x18
#define blue 0x1A

void setup() {
  Serial.begin(9600);
  Wire.begin();
  //Wire1.begin();
  myWire.begin();
  myWire1.begin();
  enable(0);
  enable(1);
  enable(2);
}

void loop() {
  Serial.print("0: "); Serial.println(getColor(0, red));
  Serial.print("1: "); Serial.println(getColor(1, red));
  Serial.print("2: ");  Serial.println(getColor(2, red));
}

void enable(int number) {
  switch (number) {
    case 0:
      Wire.beginTransmission(byte(0x29));
      Wire.write(0x80 | 0x00);
      Wire.write(0x01 & 0xFF);
      Wire.endTransmission();
      delay(3);
      Wire.beginTransmission(byte(0x29));
      Wire.write(0x80 | 0x00);
      Wire.write((0x01 | 0x02)  & 0xFF);
      Wire.endTransmission();
      break;
    case 1:
      myWire.beginTransmission(byte(0x29));
      myWire.write(0x80 | 0x00);
      myWire.write(0x01 & 0xFF);
      myWire.endTransmission();
      delay(3);
      myWire.beginTransmission(byte(0x29));
      myWire.write(0x80 | 0x00);
      myWire.write((0x01 | 0x02)  & 0xFF);
      myWire.endTransmission();
      break;
    case 2:
      myWire1.beginTransmission(byte(0x29));
      myWire1.write(0x80 | 0x00);
      myWire1.write(0x01 & 0xFF);
      myWire1.endTransmission();
      delay(3);
      myWire1.beginTransmission(byte(0x29));
      myWire1.write(0x80 | 0x00);
      myWire1.write((0x01 | 0x02)  & 0xFF);
      myWire1.endTransmission();
      break;
  }
}

unsigned int getColor(int number, byte registro) {
  uint16_t x; uint16_t t;
  switch (number) {
    case 0:
      Wire.beginTransmission(0x29);
      Wire.write(0x80 | registro);
      Wire.endTransmission();
      delay(50);
      Wire.requestFrom(0x29, 2);
      t = Wire.read();
      x = Wire.read();
      break;
    case 1:
      myWire.beginTransmission(0x29);
      myWire.write(0x80 | registro);
      myWire.endTransmission();
      delay(50);
      myWire.requestFrom(0x29, 2);
      t = myWire.read();
      x = myWire.read();
      break;
    case 2:
      myWire1.beginTransmission(0x29);
      myWire1.write(0x80 | registro);
      myWire1.endTransmission();
      delay(50);
      myWire1.requestFrom(0x29, 2);
      t = myWire1.read();
      x = myWire1.read();
      break;
  }
  x <<= 8;
  x |= t;
  return x;
}
