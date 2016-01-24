//-------------------------------------------------------------------------------
//PIN

#define M1P 2
#define M1D 3
#define M2P 4
#define M2D 5
#define M1E 6
#define M2E 7

//-------------------------------------------------------------------------------

#include <PID_v1.h>
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

//-------------------------------------------------------------------------------

int numeroMorto = 0;

//-------------------------------------------------------------------------------

void setup() {
  pinMode(M1P, OUTPUT);
  pinMode(M1D, OUTPUT);
  pinMode(M2P, OUTPUT);
  pinMode(M2D, OUTPUT);
  pinMode(M1E, OUTPUT);
  pinMode(M2E, OUTPUT);

  //-------------------------------------------------------------------------------

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz). Comment this line if having compilation difficulties with TWBR.
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(115200);

  mpu.initialize();
  devStatus = mpu.dmpInitialize();
  //-2866  -1038 1061  91  -48 12
  mpu.setXAccelOffset(-2866);
  mpu.setYAccelOffset(-1038);
  mpu.setZAccelOffset(1061);
  mpu.setXGyroOffset(91);
  mpu.setYGyroOffset(-48);
  mpu.setZGyroOffset(12);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
}

//-------------------------------------------------------------------------------

void loop() {

}

//-------------------------------------------------------------------------------
//MOVIMENTAZIONE

void avanzamento(float distanzaVoluta, float velocita, bool pid) {
  double gradiIniziali = gyroscope(0);
  float correzione = 0;
  float distanzaIniziale = distanza(0);
  float errore = 0;
  double giroscopio0 = gyroscope(0);
  double potenzaM1 = 0;
  double potenzaM2 = 0;

  if (pid) {
    double Kp = 2, Ki = 5, Kd = 1;
    PID pidM1(&giroscopio0, &potenzaM1, &gradiIniziali, Kp, Ki, Kd, DIRECT);
    PID pidM2(&giroscopio0, &potenzaM2, &gradiIniziali, Kp, Ki, Kd, DIRECT);
    pidM1.SetMode(AUTOMATIC);
    pidM2.SetMode(AUTOMATIC);

    while (distanzaIniziale - distanza(0) <= distanzaVoluta) {
      giroscopio0 = gyroscope(0);
      pidM1.Compute();
      pidM2.Compute();
      motori (velocita + potenzaM1, velocita - potenzaM2);
    }

    pidM1.SetMode(MANUAL);
    pidM2.SetMode(MANUAL);
  }

  else {
    while (distanzaIniziale - distanza(0) <= distanzaVoluta) {
      errore = gradiIniziali - gyroscope(0);
      motori (velocita + errore, velocita - errore);
    }
  }

  motori (0, 0);
}

void rotazione(float gradiVoluti, float velocita) {
  float gradiIniziali = gyroscope(0);
  float gradiFinali = gradiIniziali + gradiVoluti;

  while (gradiFinali >= gyroscope(0))
    motori (velocita, -velocita);

  motori (0, 0);
}

//-------------------------------------------------------------------------------
//GIROSCOPIO

float gyroscope(int scelta) {
  while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  mpu.getFIFOBytes(fifoBuffer, packetSize);
  fifoCount -= packetSize;

  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  return ypr[scelta] * 180 / M_PI;
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
  return 1;
}

float sensoreTemperatura () {
  return 1;
}

bool servoTorreta (int gradi) {
  return true;
}

//-------------------------------------------------------------------------------
//SERIALE VERSO SECONDO ARDUINO

void seriale(float powerMotoreA, float powerMotoreB, int numeroMorto) {
  Serial.print((int)powerMotoreA) && Serial.print(",") && Serial.print((int)powerMotoreB) && Serial.print(";") && Serial.println(numeroMorto);
}

//-------------------------------------------------------------------------------
//GESTIONE MOTORI

void motori(int powerM1, int powerM2) {
  if (powerM1 > 0) {
    analogWrite (M1D, 0);
    analogWrite (M1P, powerM1);
    digitalWrite (M1E, HIGH);
  }
  else if (powerM1 < 0) {
    analogWrite (M1D, 1);
    analogWrite (M1P, powerM1);
    digitalWrite (M1E, HIGH);
  }
  else
    digitalWrite (M1E, LOW);

  if (powerM2 > 0) {
    analogWrite (M2D, 0);
    analogWrite (M2P, powerM2);
    digitalWrite (M2E, HIGH);
  }
  else if (powerM2 < 0) {
    analogWrite (M2D, 1);
    analogWrite (M2P, powerM2);
    digitalWrite (M2E, HIGH);
  }
  else
    digitalWrite (M2E, LOW);
}
