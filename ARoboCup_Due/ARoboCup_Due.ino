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
//VARIABILI PROGRAMMA

int numeroMorto = 0;

#define sizeX 20
#define sizeY 20

unsigned int matriceLvl1[sizeX][sizeY];
unsigned int matriceLvl2[sizeX][sizeY];

//-------------------------------------------------------------------------------
//SETUP

void setup() {
  pinMode(M1E, OUTPUT);
  pinMode(M1F, OUTPUT);
  pinMode(M1R, OUTPUT);
  pinMode(M2F, OUTPUT);
  pinMode(M2R, OUTPUT);
  pinMode(M2E, OUTPUT);

  servoTorretta.attach(SM1);

  pinMode(LED, OUTPUT);
  pinMode(ENGYRO, OUTPUT);
  pinMode(ENRGB, OUTPUT);
  pinMode(ENTMP, OUTPUT);

  pixels.begin();

  //-------------------------------------------------------------------------------
  //SETUP GIROSCOPIO

  digitalWrite(ENGYRO, HIGH);
  digitalWrite(ENRGB, HIGH);
  digitalWrite(ENTMP, HIGH);

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  //  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz). Comment this line if having compilation difficulties with TWBR.
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(115200);

  mpu.initialize();
  digitalWrite(LED, LOW);
  digitalWrite(LED, mpu.testConnection() ? HIGH : LOW);

  delay(500);

  if (!mpu.testConnection()) {
    digitalWrite(ENGYRO, LOW);
    setup();
  }
  /*
    if (!tcs.begin()) {
      digitalWrite(ENRGB, LOW);
      setup();
    }

    if (!therm.begin()) {
      digitalWrite(ENTMP, LOW);
      setup();
    }
  */
  therm.setUnit(TEMP_C);

  devStatus = mpu.dmpInitialize();
  mpu.setXAccelOffset(-2766);
  mpu.setYAccelOffset(-1056);
  mpu.setZAccelOffset(1002);
  mpu.setXGyroOffset(1);
  mpu.setYGyroOffset(-15);
  mpu.setZGyroOffset(73);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
}

//-------------------------------------------------------------------------------
//LOOP

void loop() {

}

//-------------------------------------------------------------------------------
//MOVIMENTI

void muovi(int direzione) {
  direzione = direzione - gyroscope(0, false);

  switch (direzione) {
    case -3:
      destra();
      break;
    case -2:
      dietro();
      break;
    case -1:
      sinistra();
      break;
    case 0:
      avanti();
      break;
    case 1:
      destra();
      break;
    case 2:
      dietro();
      break;
    case 3:
      sinistra();
      break;
  }
}

void avanti() {
  avanzamento(30, 100);
}

void destra() {
  rotazione(90, 100);
  avanzamento(30, 100);
}

void dietro() {
  avanzamento(-30, 100);
}

void sinistra() {
  rotazione(-90, 100);
  avanzamento(30, 100);
}

//-------------------------------------------------------------------------------
//MOVIMENTAZIONE

void avanzamento(float distanzaVoluta, float velocita) {
  float gradiIniziali = gyroscope(0, true);
  //float distanzaIniziale = distanza(0, true);
  float Kp = 5;

  if (distanzaVoluta > 0) {
    int i = 0; //da cancellare
    //while (distanzaIniziale - distanza(0, true) < distanzaVoluta) {
    while (i < 100) { //da cancellare
      float errore = gradiIniziali - gyroscope(0, true);
      motori(velocita + errore * Kp, velocita - errore * Kp);
      i++;
    }
  }

  else if (distanzaVoluta < 0) {
    int i = 0; //da cancellare
    //while (distanzaIniziale - distanza(0, true) < distanzaVoluta) {
    while (i < 100) { //da cancellare
      float errore = gradiIniziali - gyroscope(0, true);
      motori(-velocita + errore * Kp, -velocita - errore * Kp);
      i++;
    }
  }

  motori (0, 0);
}

//-------------------------------------------------------------------------------

void rotazione(float gradiVoluti, float velocita) {
  float gradiIniziali = gyroscope(0, true);
  float gradiFinali = gradiIniziali + gradiVoluti;
  float K = 0.2;

  if (gradiVoluti > 0) {
    while (gradiFinali > gyroscope(0, true)) {
      float errore = gradiFinali - gyroscope(0, true);
      motori (velocita + errore * K, -velocita - errore * K);
    }
  }

  else if (gradiVoluti < 0) {
    while (gradiFinali < gyroscope(0, true)) {
      float errore = gyroscope(0, true); - gradiFinali;
      motori (-velocita - errore * K, velocita + errore * K);
    }
  }

  motori (0, 0);
}

//-------------------------------------------------------------------------------
//GIROSCOPIO

float gyroscope(int scelta, bool rotazioneContinua) {
  float misura = 0;
  for (int i = 0; i < 5; i++) {
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    misura += ypr[scelta];
  }
  misura /= 5;
  misura *= 180 / M_PI;

  float errore = previousypr[scelta] - misura;

  if (errore > 200)
    rotationypr[scelta] += 1;
  else if (errore < -200)
    rotationypr[scelta] -= 1;

  previousypr[scelta] = misura;

  if (rotazioneContinua) {
    misura += rotationypr[scelta] * 360;
    return misura;
  }
  else {
    if (misura < 10 && misura > -10)
      return 0;
    else if (misura < 100 && misura > 80)
      return 1;
    else if (misura < -170 || misura > 170)
      return 2;
    else if (misura < -80 && misura > -100)
      return 3;
  }
}

//-------------------------------------------------------------------------------
//SENSORI TORRETTA

float temperatura(int posizione, bool relativa) {
  int servo = 90;

  if (relativa) {
    switch (posizione) {
      case -1:
        servo = 0;
        break;
      case 0:
        servo = 90;
        break;
      case 1:
        servo = 180;
        break;
    }
  }

  else {
    posizione = posizione - gyroscope(0, false);
    if (posizione > 1 || posizione < -1)
      return -1;
    servo = posizione * 90 + 90;
  }

  while (!servoTorreta(servo)) {}

  float misura = sensoreTemperatura ();
  return misura;
}

//-------------------------------------------------------------------------------

float distanza(int posizione, bool relativa) {
  int servo = 90;
  int numeroSensore = 0;

  if (relativa) {
    switch (posizione) {
      case -1:
        servo = 90;
        numeroSensore = -1;
        break;
      case 0:
        servo = 90;
        numeroSensore = 0;
        break;
      case 1:
        servo = 90;
        numeroSensore = 1;
        break;
      case 2:
        servo = 180;
        numeroSensore = 1;
        break;
    }
  }

  else {
    posizione = posizione - gyroscope(0, false);
    switch (posizione) {
      case -3:
        servo = 90;
        numeroSensore = 1;
        break;
      case -2:
        servo = 0;
        numeroSensore = -1;
        break;
      case -1:
        servo = 90;
        numeroSensore = -1;
        break;
      case 0:
        servo = 90;
        numeroSensore = 0;
        break;
      case 1:
        servo = 90;
        numeroSensore = 1;
        break;
      case 2:
        servo = 180;
        numeroSensore = 1;
        break;
      case 3:
        servo = 90;
        numeroSensore = -1;
        break;
    }
  }

  while (!servoTorreta(servo)) {}

  float misura = sensoreDistanza (numeroSensore);
  return misura;
}

//-------------------------------------------------------------------------------

float sensoreDistanza(int numeroSensore) {
  float misura = 0;
  int reading = 0;
  int indirizzo = 0;

  switch (numeroSensore) {
    case -1:
      indirizzo = 112;
      break;
    case 0:
      indirizzo = 113;
      break;
    case 1:
      indirizzo = 114;
      break;
  }

  for (int i = 0; i < 5; i++) {
    Wire.beginTransmission(indirizzo);
    Wire.write(byte(0x00));
    Wire.write(byte(0x51));
    Wire.endTransmission();

    Wire.beginTransmission(indirizzo);
    Wire.write(byte(0x02));
    Wire.endTransmission();
    Wire.requestFrom(indirizzo, 2);

    if (2 <= Wire.available()) {
      reading = Wire.read();
      reading = reading << 8;
      reading |= Wire.read();
      Serial.println(reading);
    }
    misura += reading;
  }

  misura /= 5;
  return misura;
}

//-------------------------------------------------------------------------------

float sensoreTemperatura() {
  float misura = 0.0;
  for (int i = 0; i < 5; i++) {
    if (therm.read()) {
      misura += therm.object();
      misura -= therm.ambient();
    }
    misura /= 5;
    return misura;
  }
}

//-------------------------------------------------------------------------------

bool servoTorreta(int gradi) {
  servoTorretta.write(gradi);
  return true;
}

//-------------------------------------------------------------------------------
//SENSORE COLORE

bool colore(int color) {
  uint16_t clear, red, green, blue;

  tcs.setInterrupt(false);

  delay(60);

  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);

  //----------------DEBUG-------------------
  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);

  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;

  switch (color) {
    case 0:
      if (r > 210 && g > 210 && b > 210)
        return true;
      break;
    case 1:
      if (r < 10 && g < 10 && b < 10)
        return true;
      break;
    case 2:
      if (r > 180 && r < 200 && g > 180 && g < 200 && b > 180 && b < 200)
        return true;
      break;
  }
  return false;
}

//-------------------------------------------------------------------------------
//SERIALE VERSO SECONDO ARDUINO

void seriale(float powerMotoreA, float powerMotoreB, int numeroMorto) {
  Serial.print((int)powerMotoreA) && Serial.print(",") && Serial.print((int)powerMotoreB) && Serial.print(";") && Serial.println(numeroMorto);
}

//-------------------------------------------------------------------------------
//GESTIONE MOTORI

void motori(float powerM2, float powerM1) {
  powerM2 = constrain(powerM2, -255, 255);
  powerM1 = constrain(powerM1, -255, 255);

  if (powerM1 > 0) {
    analogWrite(M1F, powerM1);
    analogWrite(M1R, 0);
    digitalWrite(M1E, HIGH);
  }
  else if (powerM1 < 0) {
    analogWrite(M1F, 0);
    analogWrite(M1R, -powerM1);
    digitalWrite(M1E, HIGH);
  }
  else
    digitalWrite(M1E, LOW);

  if (powerM2 > 0) {
    analogWrite(M2F, powerM2);
    analogWrite(M2R, 0);
    digitalWrite(M2E, HIGH);
  }
  else if (powerM2 < 0) {
    analogWrite(M2F, 0);
    analogWrite(M2R, -powerM2);
    digitalWrite(M2E, HIGH);
  }
  else
    digitalWrite (M2E, LOW);
}

//-------------------------------------------------------------------------------
//GESTIONE LED

void led(int R, int G, int B, int ritardo) {
  for (int i = 0; i < 3; i++) {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
    pixels.show();
  }
  if (ritardo > 0) {
    delay (ritardo);
    for (int i = 0; i < 3; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();
    }
  }
}

//-------------------------------------------------------------------------------
//PROPRIETA' CELLA

bool propieta(int prop, int x, int y, unsigned int matrix[sizeX][sizeY]) {
  if (matrix[x][y] >= 2048) {
    if (prop == 11)
      return true;
    else
      matrix[x][y] -= 2048;
  }
  else if (prop == 11)
    return false;

  //-------------------------

  if (matrix[x][y] >= 1024) {
    if (prop == 10)
      return true;
    else
      matrix[x][y] -= 1024;
  }
  else if (prop == 10)
    return false;

  //-------------------------

  if (matrix[x][y] >= 512) {
    if (prop == 9)
      return true;
    else
      matrix[x][y] -= 512;
  }
  else if (prop == 9)
    return false;

  //-------------------------

  if (matrix[x][y] >= 256) {
    if (prop == 8)
      return true;
    else
      matrix[x][y] -= 256;
  }
  else if (prop == 8)
    return false;

  //-------------------------

  if (matrix[x][y] >= 128) {
    if (prop == 7)
      return true;
    else
      matrix[x][y] -= 128;
  }
  else if (prop == 7)
    return false;

  //-------------------------

  if (matrix[x][y] >= 64) {
    if (prop == 6)
      return true;
    else
      matrix[x][y] -= 64;
  }
  else if (prop == 6)
    return false;

  //-------------------------

  if (matrix[x][y] >= 32) {
    if (prop == 5)
      return true;
    else
      matrix[x][y] -= 32;
  }
  else if (prop == 5)
    return false;

  //-------------------------

  if (matrix[x][y] >= 16) {
    if (prop == 4)
      return true;
    else
      matrix[x][y] -= 16;
  }
  else if (prop == 4)
    return false;

  //-------------------------

  if (matrix[x][y] >= 8) {
    if (prop == 3)
      return true;
    else
      matrix[x][y] -= 8;
  }
  else if (prop == 3)
    return false;

  //-------------------------

  if (matrix[x][y] >= 4) {
    if (prop == 2)
      return true;
    else
      matrix[x][y] -= 4;
  }
  else if (prop == 2)
    return false;

  //-------------------------

  if (matrix[x][y] >= 2) {
    if (prop == 1)
      return true;
    else
      matrix[x][y] -= 2;
  }
  else if (prop == 1)
    return false;

  //-------------------------

  if (matrix[x][y] >= 1) {
    if (prop == 0)
      return true;
  }
  else if (prop == 0)
    return false;
}

//-------------------------------------------------------------------------------
//MAPPATURA

void mappatura(int x, int y, int livello) {
  int prop = 0;
  if (livello == 1)
    prop = matriceLvl1[x][y];
  else if (livello == 2)
    prop = matriceLvl2[x][y];

  if (prop == 0) {
    prop += 1;

    if (distanza(0, false) < 20) {
      prop += 2;
      if (temperatura(0, false) > 8)
        prop += 32;
    }
    if (distanza(1, false) < 20) {
      prop += 4;
      if (temperatura(1, false) > 8)
        prop += 64;
    }
    if (distanza(2, false) < 20) {
      prop += 8;
      if (temperatura(2, false) > 8)
        prop += 128;
    }
    if (distanza(3, false) < 20) {
      prop += 16;
      if (temperatura(3, false) > 8)
        prop += 256;
    }

    if (colore(1))
      prop += 1024;
    if (colore(2))
      prop += 2048;
  }

  if (livello == 1)
    matriceLvl1[x][y] = prop;
  else if (livello == 2)
    matriceLvl2[x][y] = prop;
}

//-------------------------------------------------------------------------------
