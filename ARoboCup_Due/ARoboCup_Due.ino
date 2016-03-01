//-------------------------------------------------------------------------------
//PIN

//Servo Motori
#define SM1 10

//Alimentazione Sensori
#define ENGYRO 24
#define ENRGB 25
#define ENTMP 26
#define LED 13

//Motori
#define M1E 7
#define M1F 6
#define M1R 5
#define M2F 4
#define M2R 3
#define M2E 2

//-------------------------------------------------------------------------------
//SERVO

#include <Servo.h>
Servo servoTorretta;

//-------------------------------------------------------------------------------
//GIROSCOPIO

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;

bool dmpReady = false;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

float previousypr[3];
int rotationypr[3];

byte gammatable[256];

//-------------------------------------------------------------------------------
//STRISCIA LED

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, 22, NEO_GRB + NEO_KHZ800);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

//-------------------------------------------------------------------------------
//TERMOMETRO INFRAROSSI

#include <SparkFunMLX90614.h>
IRTherm therm;

//-------------------------------------------------------------------------------
//LCD

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//-------------------------------------------------------------------------------
//SCHEDULER

#include <Scheduler.h>

//-------------------------------------------------------------------------------
//VARIABILI PROGRAMMA

int actualX = 25;
int actualY = 25;
int actualLevel = 1;

#define sizeX 50
#define sizeY 50

unsigned int matriceLvl1[sizeX][sizeY];
unsigned int matriceLvl2[sizeX][sizeY];

short passaggiLvl1[sizeX][sizeY];
short passaggiLvl2[sizeX][sizeY];

int kitCounter = 15;

//-------------------------------------------------------------------------------
//SETUP

void setup() {
  Serial.begin(115200);
  pinSetup();
  sensorSetup();

  Scheduler.startLoop(seriale);
}

//-------------------------------------------------------------------------------
//LOOP

void loop() {
  program();
  yield();
}
