//-------------------------------------------------------------------------------
//PIN

#define button 53

//Servo Motore 1 Torretta
#define SM1 8
#define timeSM1 10
#define posSXSM1 180
#define posAVSM1 100
#define posDXSM1 7
int posizioneSM1 = posAVSM1;

//Servo Motore 2 SX
#define SM2 11
#define timeSM2 5
#define posDownSM2 5
#define posCloseSM2 35
#define posOpenSM2 55
int posizioneSM2 = posCloseSM2;

//Servo Motore 3 DX
#define SM3 10
#define timeSM3 5
#define posDownSM3 175
#define posCloseSM3 140
#define posOpenSM3 110
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
#define resetNano 22

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
#define pRampaPosizione0 12
#define pRampaPosizione1 13
#define pRampaPosizione2 14
#define pRampaPosizione3 15

//-------------------------------------------------------------------------------
//SERVO

#include <Servo.h>
Servo servoTorretta;
Servo servoDispenserSX;
Servo servoDispenserDX;
Servo beep;

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

#define differenzaTemperatura 1;
int sogliaTemperatura = 0;

//-------------------------------------------------------------------------------
//SRF10

int gain = 6;
byte range = 0x5D;

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

bool rampa = true;
int cellCounter = 0;

#define tempoRampa 5000

//-------------------------------------------------------------------------------
//SETUP

void setup() {
  //overclock();

  for (int i = 0; i < sizeX; i++) {
    for (int j = 0; j < sizeY; j++) {
      mappa[i][j][0] = 0;
      mappa[i][j][1] = 0;
      passaggi[i][j][0] = 0;
      passaggi[i][j][1] = 0;
    }
  }

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

  light(100, 6);
  go_on();

  //Scheduler.startLoop(seriale);
  //azzeraCulo();
}

//-------------------------------------------------------------------------------
//LOOP

void loop() {
  program();
  yield();
}

void overclock() {  //Use this (change from 18UL for 114MHz to 15UL for 96MHz, 84MHz is 13UL or 0x0dUL (as in system_sam3xa.c):
#define SYS_BOARD_PLLAR (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(15UL) | CKGR_PLLAR_PLLACOUNT(0x3fUL) | CKGR_PLLAR_DIVA(1UL))
#define SYS_BOARD_MCKR ( PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK)

  //Set FWS according to SYS_BOARD_MCKR configuration
  EFC0->EEFC_FMR = EEFC_FMR_FWS(4); //4 waitstate flash access
  EFC1->EEFC_FMR = EEFC_FMR_FWS(4);

  // Initialize PLLA to 114MHz
  PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
  while (!(PMC->PMC_SR & PMC_SR_LOCKA)) {}

  PMC->PMC_MCKR = SYS_BOARD_MCKR;
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {}

  SystemCoreClockUpdate();

  /*#define SYS_BOARD_PLLAR (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(15UL) | CKGR_PLLAR_PLLACOUNT(0x3fUL) | CKGR_PLLAR_DIVA(1UL))
    #define SYS_BOARD_MCKR ( PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK)

    // Set FWS according to SYS_BOARD_MCKR configuration
    EFC0->EEFC_FMR = EEFC_FMR_FWS(4); //4 waitstate flash access
    EFC1->EEFC_FMR = EEFC_FMR_FWS(4);

    // Initialize PLLA to (15+1)*6=96MHz
    PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
    while (!(PMC->PMC_SR & PMC_SR_LOCKA)) {}

    PMC->PMC_MCKR = SYS_BOARD_MCKR;
    while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {}*/
}
