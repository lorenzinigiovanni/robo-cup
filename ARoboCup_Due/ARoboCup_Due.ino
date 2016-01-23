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

  avanzamento(100, 60);
}

//-------------------------------------------------------------------------------

void loop() {

}

//-------------------------------------------------------------------------------

void avanzamentoPID(float distanzaVoluta, float velocita) {
  float gradiIniziali = gyroscope(0);
  float correzione = 0;
  float distanzaIniziale = distanza();
  float errore = 0;
  double Setpoint, Input, Output;
  double Kp = 2, Ki = 5, Kd = 1;
  PID pidAvanzamento(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
  pidAvanzamento.SetMode(AUTOMATIC);

  Serial.print("start deg: ") && Serial.println(gradiIniziali);

  if (gradiIniziali < 90) {
    gradiIniziali += 90;
    correzione = 90;
  }
  else if (gradiIniziali > 270) {
    gradiIniziali -= 90;
    correzione = 90;
  }

  Serial.print("yaw: ") && Serial.println(gradiIniziali);

  while (distanzaIniziale - distanza() <= distanzaVoluta) {
    pidAvanzamento.Compute();
    errore = gradiIniziali - gyroscope(0);
    Serial.print("errore: ") && Serial.print(errore) && Serial.print("\t");
    Serial.print("delta D: ") && Serial.println(distanzaIniziale - distanza());
    seriale (velocita - errore, velocita + errore, numeroMorto);
  }

  seriale (0, 0, numeroMorto);
}

//-------------------------------------------------------------------------------

void avanzamento(float distanzaVoluta, float velocita) {
  float gradiIniziali = gyroscope(0);
  float correzione = 0;
  //  float distanzaIniziale = distanza();
  float errore = 0;

  //  if (gradiIniziali < 90) {
  //    gradiIniziali += 90;
  //    correzione = 90;
  //  }
  //  else if (gradiIniziali > 270) {
  //    gradiIniziali -= 90;
  //    correzione = 90;
  //  }

  //while (distanzaIniziale - distanza() <= distanzaVoluta) {
  while (true) {
    errore = gradiIniziali - gyroscope(0);
//    Serial.println(gyroscope(0));
    seriale (velocita + errore, velocita - errore, numeroMorto);
  }

  seriale (0, 0, numeroMorto);
}

//-------------------------------------------------------------------------------

void rotazione(float gradiVoluti, float velocita) {
  float gradiIniziali = gyroscope(0);

  if (gradiIniziali > 180) {
    float gradiFinali = gradiIniziali - gradiVoluti;
    while (gradiFinali <= gyroscope(0)) {
      float errore = gradiFinali - gyroscope(0);
      seriale (-velocita, velocita, numeroMorto);
    }
  }

  else {
    float gradiFinali = gradiIniziali + gradiVoluti;
    while (gradiFinali >= gyroscope(0)) {
      seriale (velocita, -velocita, numeroMorto);
    }
  }

  seriale (0, 0, numeroMorto);
}

//-------------------------------------------------------------------------------

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

float distanza() {
  return 1;
}

//-------------------------------------------------------------------------------

void seriale(float powerMotoreA, float powerMotoreB, int numeroMorto) {
  Serial.print((int)powerMotoreA) && Serial.print(",") && Serial.print((int)powerMotoreB) && Serial.print(";") && Serial.println(numeroMorto);
}

