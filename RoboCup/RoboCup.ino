//-------------------------------------------------------------------------------
//PIN

//Servo Motori
#define SM1 10

//Alimentazione Sensori
#define ENGYRO 24
#define ENRGB 25
#define ENTMP1 26
#define ENTMP2 27
#define LED 13
#define colorLED 12

//Motori
#define M1E 7
#define M1F 6
#define M1R 5
#define M2F 3
#define M2R 4
#define M2E 2

//Striscia LED
#define strisciaPin 8
#define strisciaLed 3

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

int16_t ax, ay, az, gx, gy, gz;
int mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz, state = 0;
int ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;

//-------------------------------------------------------------------------------
//STRISCIA LED

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(strisciaLed, strisciaPin, NEO_GRB + NEO_KHZ800);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

//-------------------------------------------------------------------------------
//TERMOMETRO INFRAROSSI

static const uint32_t TWI_CLOCK = 100000;
static const uint32_t RECV_TIMEOUT = 100000;
static const uint32_t XMIT_TIMEOUT = 100000;

#include <Arduino.h>
#include <include/twi.h>
Twi *pTwi = WIRE1_INTERFACE;

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
  sensorCalibration();
  Scheduler.startLoop(seriale);
}

//-------------------------------------------------------------------------------
//LOOP

void loop() {
  program();
  yield();
}
