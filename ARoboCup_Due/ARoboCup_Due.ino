//-------------------------------------------------------------------------------
//PIN

#define sizeX 20
#define sizeY 20

//Servo Motori
#define SM1 10

//Motori
#define M1E 8
#define M1F 7
#define M1R 6
#define M2F 5
#define M2R 4
#define M2E 3

//-------------------------------------------------------------------------------

#include <Servo.h>
Servo servoTorretta;

//-------------------------------------------------------------------------------

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

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

unsigned int matriceLvl1[sizeX][sizeY];
unsigned int matriceLvl2[sizeX][sizeY];

//-------------------------------------------------------------------------------

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, 22, NEO_GRB + NEO_KHZ800);

//-------------------------------------------------------------------------------

#include <SparkFunMLX90614.h>
IRTherm therm;

//-------------------------------------------------------------------------------

int numeroMorto = 0;

//-------------------------------------------------------------------------------

void setup() {
  pinMode(M1E, OUTPUT);
  pinMode(M1F, OUTPUT);
  pinMode(M1R, OUTPUT);
  pinMode(M2F, OUTPUT);
  pinMode(M2R, OUTPUT);
  pinMode(M2E, OUTPUT);

  servoTorretta.attach(SM1);

  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);

  pixels.begin();

  //-------------------------------------------------------------------------------

  digitalWrite(12, HIGH);

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  //  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz). Comment this line if having compilation difficulties with TWBR.
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(115200);

  mpu.initialize();
  digitalWrite(13, LOW);
  digitalWrite(13, mpu.testConnection() ? HIGH : LOW);

  delay(500);

  if (!mpu.testConnection()) {
    digitalWrite(12, LOW);
    setup();
  }

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

  //-------------------------------------------------------------------------------

  therm.begin();
  therm.setUnit(TEMP_C);

}

//-------------------------------------------------------------------------------

void loop() {

}

//-------------------------------------------------------------------------------
//MOVIMENTAZIONE

void avanzamento(float distanzaVoluta, float velocita) {
  float gradiIniziali = gyroscope(0, true);;
  float correzione = 0;
  //  float distanzaIniziale = distanza(0);
  float errore = 0;
  float Kp = 5;
  int i = 0;
  while (i < 100) {
    //while (distanzaIniziale - distanza(0) <= distanzaVoluta) {
    errore = gradiIniziali - gyroscope(0, true);;
    //Serial.print("ERRORE= ") && Serial.print(errore)  && Serial.print("\t GYRO= ") && Serial.println(gyroscope(0));

    motori(velocita + errore * Kp, velocita - errore * Kp);
    i++;
  }

  motori (0, 0);
}

void rotazione(float gradiVoluti, float velocita) {
  float gradiIniziali = gyroscope(0, true);
  float gradiFinali = gradiIniziali + gradiVoluti;
  float K = 0.2;

  if (gradiVoluti > 0) {
    while (gradiFinali >= gyroscope(0, true)) {
      float errore = gradiFinali - gyroscope(0, true);
      motori (velocita + errore * K, -velocita - errore * K);
    }
  }
  else if (gradiVoluti < 0) {
    while (gradiFinali <= gyroscope(0, true)) {
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
  else
    return misura;
}

//-------------------------------------------------------------------------------
//SENSORI TORRETTA

float temperatura(int gradiMisura) {
  int servo = 90;
  servo += gradiMisura;

  while (!servoTorreta(servo)) {}

  float misura = sensoreTemperatura ();
  return misura;
}

float distanza(int gradiMisura) {
  int numeroSensore = 0;
  int servo = 90;
  if (gradiMisura <= -90) {
    numeroSensore = -1;
    servo += gradiMisura + 90;
  }
  else if (gradiMisura >= 90) {
    numeroSensore = 1;
    servo += gradiMisura - 90;
  }
  else {
    numeroSensore = 0;
    servo += gradiMisura;
  }

  while (!servoTorreta(servo)) {}

  float misura = sensoreDistanza (numeroSensore);
  return misura;
}

float sensoreDistanza (int numeroSensore) {
  float misura = 1;
  for (int i = 0; i < 5; i++) {
    misura += misura;
  }
  misura /= 5;
  return misura;
}

float sensoreTemperatura () {
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

bool servoTorreta (int gradi) {
  servoTorretta.write(gradi);
  return true;
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
//DESMAPPATURA

bool mappa(int prop, int x, int y, unsigned int matrix[sizeX][sizeY]) {
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
