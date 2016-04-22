//-------------------------------------------------------------------------------
//PIN

//Servo Motore 1 Torretta
#define SM1 9
#define timeSM1 5
#define posSXSM1 180
#define posAVSM1 88
#define posDXSM1 5
int posizioneSM1 = posAVSM1;

//Servo Motore 2 SX
#define SM2 10
#define timeSM2 5
#define posDownSM2 180
#define posCloseSM2 150
#define posOpenSM2 130
int posizioneSM2 = posCloseSM2;

//Servo Motore 3 DX
#define SM3 11
#define timeSM3 5
#define posDownSM3 0
#define posCloseSM3 30
#define posOpenSM3 50
int posizioneSM3 = posCloseSM3;

//Alimentazione Sensori
#define ENRGB 25
#define ENTMP 26
#define LED 13
#define colorLED 12
#define powerColorLED 255

//Motori
#define M1E 7
#define M1F 6
#define M1R 5
#define M2F 3
#define M2R 4
#define M2E 2

//Arduino Nano
#define resetNano 28

//-------------------------------------------------------------------------------
//PROPRIETA

#define pCellaVisitata 0
#define pMuroPosizione0 1
#define pMuroPosizione1 2
#define pMuroPosizione2 3
#define pMuroPosizione3 4
#define pVittimaPosizione0 5
#define pVittimaPosizione1 6
#define pVittimaPosizione2 7
#define pVittimaPosizione3 8
#define pVittimeSalvate 9
#define pCellaNera 10
#define pCellaGrigia 11

//-------------------------------------------------------------------------------
//SERVO

#include <Servo.h>
Servo servoTorretta;
Servo servoDispenserSX;
Servo servoDispenserDX;

//-------------------------------------------------------------------------------
//GIROSCOPIO

#define gyroscopeTimeOut 1000
#define gyroscopeCalibrationTimeOut 100000
#define errorePosizioni 20

float gyroArray[6];

//-------------------------------------------------------------------------------
//I2C

#include "I2Cdev.h"
#include "Wire.h"

//-------------------------------------------------------------------------------
//COLOR

#include "Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
float Kred = 1;
float Kgreen = 1;
float Kblue = 1;

//-------------------------------------------------------------------------------
//TERMOMETRO INFRAROSSI

static const uint32_t TWI_CLOCK = 100000;
static const uint32_t RECV_TIMEOUT = 100000;
static const uint32_t XMIT_TIMEOUT = 100000;

#include <Arduino.h>
#include <include/twi.h>
Twi *pTwi = WIRE1_INTERFACE;

#define differenzaTemperatura 5;
int sogliaTemperatura = 0;

//-------------------------------------------------------------------------------
//SRF10

int gain = 0;
int range = 255;

//-------------------------------------------------------------------------------
//SCHEDULER

#include <Scheduler.h>

//-------------------------------------------------------------------------------
//VARIABILI PROGRAMMA

int actualX = 25;
int actualY = 25;
int actualL = 0;
int previousX = 25;
int previousY = 25;
int previousL = 0;

#define sizeX 50
#define sizeY 50
#define sizeL 2

unsigned int mappa[sizeX][sizeY][sizeL];
short passaggi[sizeX][sizeY][sizeL];

int kitCounter = 12;
bool kitPosition = false;

//-------------------------------------------------------------------------------
//SETUP

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  pinSetup();
  Serial.println("pinSetup OK");
  resetArduinoNano();
  Serial.println("resetArduinoNano OK");
  sensorSetup();
  Serial.println("sensorSetup OK");
  sensorCalibration();
  Serial.println("sensorCalibration OK");

  long unsigned int time = millis();
  while (true) {
    if (millis() - time > gyroscopeCalibrationTimeOut) {
      resetArduinoNano();
      time = millis();
    }
    if (Serial1.available())
      if (Serial1.readStringUntil('\n').indexOf("OK") != -1)
        break;
  }
  
Serial.println("Gyroscope OK");

  Scheduler.startLoop(seriale);
}

//-------------------------------------------------------------------------------
//LOOP

void loop() {
  program();
  yield();
}
