#include "MeOrion.h"
#include <SoftwareSerial.h>

#define ULTRASONIC_SENSOR 1
//#define RGBLED 8
#define MOTOR 10
#define ULTRASONIC_ARDUINO 36
#define GET 1
#define RUN 2
#define RESET 4
#define START 5

union {
  byte byteVal[2];
  short shortVal;
} valShort;

union {
  byte byteVal[4];
  float floatVal;
  long longVal;
} val;

int len = 52;
char buffer[52];
char bufferBt[52];

bool isBluetooth = true;

MeBluetooth bluetooth(PORT_5);

MeDCMotor Pinza(PORT_1);
MeDCMotor Braccio(PORT_2);
MeDCMotor cingoloA(M1);
MeDCMotor cingoloB(M2);
MeRGBLed led(PORT_6);

MeUltrasonicSensor ultraSensor(PORT_3);

unsigned char table[128] = {0};

void setup() {
  bluetooth.begin(115200);
  Serial.begin(115200);
  ledColors(255, 0, 255);
}

void loop() {
  /*speedMotor(cingoloA, -distanza());
    speedMotor(cingoloB, distanza());*/

  int readdata = 0, i = 0, count = 0;
  char outDat;
  if (bluetooth.available())
  {
    while ((readdata = bluetooth.read()) != (int) - 1)
    {
      table[count] = readdata;
      count++;
      delayMicroseconds(100);
    }
    for (i = 0; i < count; i++)
      writeBuffer(i, table[i]);
    parseData();
    Serial.print(distanza()) && Serial.println(" CM"); //DEBUG
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

void ledColor(int idx, int r, int g, int b) {
  led.setColorAt(idx, r, g, b);

  led.show();
}

/*
  ff 55 len idx action device port  slot  data a
  0  1  2   3   4      5      6     7     8
*/

void parseData() {
  int idx = readBuffer(3);
  int action = readBuffer(4);
  int device = readBuffer(5);

  switch (action) {
    case GET:
      writeHead();
      writeSerial(idx);
      readSensor(device);
      writeEnd();
      break;
    case RUN:
      runModule(device);
      break;
    case RESET:
      //reset();
      break;
    case START:
      callOk();
      break;
  }
}

unsigned char readBuffer(int index) {
  return isBluetooth ? bufferBt[index] : buffer[index];
}

void writeBuffer(int index, unsigned char c) {
  if (isBluetooth)
    bufferBt[index] = c;
  else
    buffer[index] = c;
}

void runModule(int device) {
  //0xff 0x55 0x6 0x0 0x1 0xa 0x9 0x0 0x0 0xa
  int port = readBuffer(6);
  int pin = port;

  switch (device) {
    case MOTOR: //{
      int speed = readShort(7);
      //dc.reset(port);
      if (port == 1)
        speedMotor(Pinza, speed);
      else if (port == 2)
        speedMotor(Braccio, speed);
      else if (port == 9)
        speedMotor(cingoloA, speed);
      else if (port == 10)
        speedMotor(cingoloB, speed);
      break;
      //      }
      /*case RGBLED: IMPALLA TUTTO!!!
        int slot = readBuffer(7);
        int idx = readBuffer(8);
        int r = readBuffer(9);
        int g = readBuffer(10);
        int b = readBuffer(11);
        //led.reset(port, slot);
        if (idx > 0)
          ledColor(idx - 1, r, g, b);
        else
          ledColors(r, g, b);
        break;*/
  }
}

void readSensor(int device) {
  /**************************************************
      ff 55 len idx action device port slot data a
      0  1  2   3   4      5      6    7    8
  ***************************************************/
  float value = 0.0;
  int port, slot, pin;
  port = readBuffer(6);
  pin = port;
  switch (device) {
    case  ULTRASONIC_SENSOR:
      /*if (us.getPort() != port) {
        us.reset(port);
        }
        value = us.distanceCm();
        sendFloat(value);*/
      sendFloat(distanza());
      break;/*
    case  TEMPERATURE_SENSOR: {
        slot = readBuffer(7);
        if (ts.getPort() != port || ts.getSlot() != slot) {
          ts.reset(port, slot);
        }
        value = ts.temperature();
        sendFloat(value);
      }
      break;
    case  LIGHT_SENSOR:
    case  SOUND_SENSOR:
    case  POTENTIONMETER: {
        if (generalDevice.getPort() != port) {
          generalDevice.reset(port);
          pinMode(generalDevice.pin2(), INPUT);
        }
        value = generalDevice.aRead2();
        sendFloat(value);
      }
      break;
    case  JOYSTICK: {
        slot = readBuffer(7);
        if (joystick.getPort() != port) {
          joystick.reset(port);
        }
        value = joystick.read(slot);
        sendFloat(value);
      }
      break;
    case  PIRMOTION: {
        if (generalDevice.getPort() != port) {
          generalDevice.reset(port);
          pinMode(generalDevice.pin2(), INPUT);
        }
        value = generalDevice.dRead2();
        sendFloat(value);
      }
      break;
    case  LINEFOLLOWER: {
        if (generalDevice.getPort() != port) {
          generalDevice.reset(port);
          pinMode(generalDevice.pin1(), INPUT);
          pinMode(generalDevice.pin2(), INPUT);
        }
        value = generalDevice.dRead1() * 2 + generalDevice.dRead2();
        sendFloat(value);
      }
      break;
    case LIMITSWITCH: {
        slot = readBuffer(7);
        if (generalDevice.getPort() != port || generalDevice.getSlot() != slot) {
          generalDevice.reset(port, slot);
        }
        if (slot == 1) {
          pinMode(generalDevice.pin1(), INPUT_PULLUP);
          value = generalDevice.dRead1();
        } else {
          pinMode(generalDevice.pin2(), INPUT_PULLUP);
          value = generalDevice.dRead2();
        }
        sendFloat(value);
      }
      break;
    //   case COMPASS:{
    //     if(Compass.getPort()!=port){
    //       Compass.reset(port);
    //       Compass.setpin(Compass.pin1(),Compass.pin2());
    //     }
    //     double CompassAngle;
    //     CompassAngle = Compass.getAngle();
    //     sendDouble(CompassAngle);
    //   }
    //   break;
    case HUMITURE: {
        uint8_t index = readBuffer(7);
        if (humiture.getPort() != port) {
          humiture.reset(port);
        }
        uint8_t HumitureData;
        humiture.update();
        HumitureData = humiture.getValue(index);
        sendByte(HumitureData);
      }
      break;
    case FLAMESENSOR: {
        if (FlameSensor.getPort() != port) {
          FlameSensor.reset(port);
          FlameSensor.setpin(FlameSensor.pin2(), FlameSensor.pin1());
        }
        int16_t FlameData;
        FlameData = FlameSensor.readAnalog();
        sendShort(FlameData);
      }
      break;
    case GASSENSOR: {
        if (GasSensor.getPort() != port) {
          GasSensor.reset(port);
          GasSensor.setpin(GasSensor.pin2(), GasSensor.pin1());
        }
        int16_t GasData;
        GasData = GasSensor.readAnalog();
        sendShort(GasData);
      }
      break;
    case  VERSION: {
        sendString(mVersion);
      }
      break;
    case  DIGITAL: {
        pinMode(pin, INPUT);
        sendFloat(digitalRead(pin));
      }
      break;
    case  ANALOG: {
        pin = analogs[pin];
        pinMode(pin, INPUT);
        sendFloat(analogRead(pin));
      }
      break;
    case  PULSEIN: {
        int pw = readShort(7);
        pinMode(pin, INPUT);
        sendShort(pulseIn(pin, HIGH, pw));
      }
      break;*/
    case ULTRASONIC_ARDUINO:
      //      int trig = readBuffer(6);
      //      int echo = readBuffer(7);
      //      pinMode(trig, OUTPUT);
      //      digitalWrite(trig, LOW);
      //      delayMicroseconds(2);
      //      digitalWrite(trig, HIGH);
      //      delayMicroseconds(10);
      //      digitalWrite(trig, LOW);
      //      pinMode(echo, INPUT);
      //      sendFloat(pulseIn(echo, HIGH, 30000) / 58.0);
      sendFloat(distanza());
      break;
  }
}

short readShort(int idx) {
  valShort.byteVal[0] = readBuffer(idx);
  valShort.byteVal[1] = readBuffer(idx + 1);
  return valShort.shortVal;
}

void sendFloat(float value) {
  writeSerial(0x2);
  val.floatVal = value;
  writeSerial(val.byteVal[0]);
  writeSerial(val.byteVal[1]);
  writeSerial(val.byteVal[2]);
  writeSerial(val.byteVal[3]);
}

void writeSerial(unsigned char c) {
  Serial.write(c);
#if defined(__AVR_ATmega32U4__)
  Serial1.write(c);
#endif
}

void callOk() {
  writeHead();
  writeEnd();
}

void writeHead() {
  writeSerial(0xff);
  writeSerial(0x55);
}

void writeEnd() {
  Serial.println();
#if defined(__AVR_ATmega_32U4__)
  Serial1.println();
#endif
}

