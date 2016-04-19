//-------------------------------------------------------------------------------
//PIN

//Servo Motore 1
#define SM1 9
#define timeSM1 5
#define posSXSM1 0
#define posAVSM1 90
#define posDXSM1 180
int posizioneSM1 = 90;

//Servo Motore 2
#define SM2 9
#define timeSM2 5
#define posSXSM2 0
#define posAVSM2 90
#define posDXSM2 180
int posizioneSM2 = 90;

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
Servo servoDispenser;

//-------------------------------------------------------------------------------
//GIROSCOPIO

#define gyroscopeTimeOut 1000
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

//-------------------------------------------------------------------------------
//TERMOMETRO INFRAROSSI

static const uint32_t TWI_CLOCK = 100000;
static const uint32_t RECV_TIMEOUT = 100000;
static const uint32_t XMIT_TIMEOUT = 100000;

#include <Arduino.h>
#include <include/twi.h>
Twi *pTwi = WIRE1_INTERFACE;

#define sogliaTemperatura 5

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

int kitCounter = 8;
bool kitPosition = false;

//-------------------------------------------------------------------------------
//SETUP

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  pinSetup();
  sensorSetup();
  sensorCalibration();

  while (true)
    if (Serial1.available())
      if (Serial1.readStringUntil('\n').indexOf("OK") != -1)
        break;

  Scheduler.startLoop(seriale);
}

//-------------------------------------------------------------------------------
//LOOP

void loop() {
  program();
  yield();
}
