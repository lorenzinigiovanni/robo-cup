#define errorePosizioni 20

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
float misura[3];

int16_t ax, ay, az, gx, gy, gz;
int mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz, state = 0;
int ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;

//-------------------------------------------------------------------------------

void setup() {
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  //  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz). Comment this line if having compilation difficulties with TWBR.
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(115200);

  mpu.initialize();

  pinMode(10, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //calGiroscopio(100, 8, 1);

  devStatus = mpu.dmpInitialize();

  /*
    mpu.setXAccelOffset(ax_offset);
    mpu.setYAccelOffset(ay_offset);
    mpu.setZAccelOffset(az_offset);
    mpu.setXGyroOffset(gx_offset);
    mpu.setYGyroOffset(gy_offset);
    mpu.setZGyroOffset(gz_offset);
  */

  mpu.setXAccelOffset(-2825);
  mpu.setYAccelOffset(-1054);
  mpu.setZAccelOffset(1106);
  mpu.setXGyroOffset(92);
  mpu.setYGyroOffset(-61);
  mpu.setZGyroOffset(15);


  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  }

  Serial.println("OK");
}

//-------------------------------------------------------------------------------

void loop() {
  if (!dmpReady) return;

  leggi();

  if (Serial.available()) {
    Serial.readStringUntil(10);
    String tmp = "";
    tmp += String(gyroscope(0, false));
    tmp += ",";
    tmp += String(gyroscope(1, false));
    tmp += ",";
    tmp += String(gyroscope(2, false));
    tmp += ",";
    tmp += String(gyroscope(0, true));
    tmp += ",";
    tmp += String(gyroscope(1, true));
    tmp += ",";
    tmp += String(gyroscope(2, true));
    tmp += ",";
    Serial.println(tmp);
  }
}

//-------------------------------------------------------------------------------

void leggi() {
  while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  mpu.getFIFOBytes(fifoBuffer, packetSize);

  fifoCount -= packetSize;

  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  for (int scelta = 0; scelta < 3; scelta++) {
    misura[scelta] = ypr[scelta];
    misura[scelta] *= 180 / M_PI;

    float errore = previousypr[scelta] - misura[scelta];

    if (errore > 200)
      rotationypr[scelta] += 1;
    else if (errore < -200)
      rotationypr[scelta] -= 1;

    previousypr[scelta] = misura[scelta];
  }
}

//-------------------------------------------------------------------------------

float gyroscope(int scelta, bool rotazioneContinua) {
  if (rotazioneContinua) {
    misura[scelta] += rotationypr[scelta] * 360;
    return misura[scelta];
  }
  else {
    if (misura[scelta] < errorePosizioni && misura[scelta] > -errorePosizioni)
      return 0;
    else if (misura[scelta] < 90 + errorePosizioni && misura[scelta] > 90 - errorePosizioni)
      return 1;
    else if (misura[scelta] < -180 + errorePosizioni || misura[scelta] > 180 - errorePosizioni)
      return 2;
    else if (misura[scelta] < -90 + errorePosizioni && misura[scelta] > -90 - errorePosizioni)
      return 3;
  }
}

//-------------------------------------------------------------------------------

void calGiroscopio(int buffersize, int acel_deadzone, int giro_deadzone) {
  if (state == 0) {
    calGiroscopioMeansensors(buffersize, acel_deadzone, giro_deadzone);
    state++;
    delay(1000);
  }
  if (state == 1) {
    calGiroscopioCalibration(buffersize, acel_deadzone, giro_deadzone);
    state++;
    delay(1000);
  }
  if (state == 2) {
    calGiroscopioMeansensors(buffersize, acel_deadzone, giro_deadzone);
  }
}

//-------------------------------------------------------------------------------

void calGiroscopioMeansensors(int buffersize, int acel_deadzone, int giro_deadzone) {
  long i = 0, buff_ax = 0, buff_ay = 0, buff_az = 0, buff_gx = 0, buff_gy = 0, buff_gz = 0;

  while (i < (buffersize + 101)) {
    // read raw accel/gyro measurements from device
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    if (i > 100 && i <= (buffersize + 100)) { //First 100 measures are discarded
      buff_ax = buff_ax + ax;
      buff_ay = buff_ay + ay;
      buff_az = buff_az + az;
      buff_gx = buff_gx + gx;
      buff_gy = buff_gy + gy;
      buff_gz = buff_gz + gz;
    }
    if (i == (buffersize + 100)) {
      mean_ax = buff_ax / buffersize;
      mean_ay = buff_ay / buffersize;
      mean_az = buff_az / buffersize;
      mean_gx = buff_gx / buffersize;
      mean_gy = buff_gy / buffersize;
      mean_gz = buff_gz / buffersize;
    }
    i++;
    delay(2); //Needed so we don't get repeated measures
  }
}

//-------------------------------------------------------------------------------

void calGiroscopioCalibration(int buffersize, int acel_deadzone, int giro_deadzone) {
  ax_offset = -mean_ax / 8;
  ay_offset = -mean_ay / 8;
  az_offset = (16384 - mean_az) / 8;

  gx_offset = -mean_gx / 4;
  gy_offset = -mean_gy / 4;
  gz_offset = -mean_gz / 4;
  while (1) {
    int ready = 0;
    mpu.setXAccelOffset(ax_offset);
    mpu.setYAccelOffset(ay_offset);
    mpu.setZAccelOffset(az_offset);

    mpu.setXGyroOffset(gx_offset);
    mpu.setYGyroOffset(gy_offset);
    mpu.setZGyroOffset(gz_offset);

    calGiroscopioMeansensors(buffersize, acel_deadzone, giro_deadzone);

    Serial.println("...");

    Serial.print(mean_ax);
    Serial.print("\t");
    Serial.print(mean_ay);
    Serial.print("\t");
    Serial.print(mean_az);
    Serial.print("\t");
    Serial.print(mean_gx);
    Serial.print("\t");
    Serial.print(mean_gy);
    Serial.print("\t");
    Serial.println(mean_gz);

    if (abs(mean_ax) <= acel_deadzone) ready++;
    else ax_offset = ax_offset - mean_ax / acel_deadzone;

    if (abs(mean_ay) <= acel_deadzone) ready++;
    else ay_offset = ay_offset - mean_ay / acel_deadzone;

    if (abs(16384 - mean_az) <= acel_deadzone) ready++;
    else az_offset = az_offset + (16384 - mean_az) / acel_deadzone;

    if (abs(mean_gx) <= giro_deadzone) ready++;
    else gx_offset = gx_offset - mean_gx / (giro_deadzone + 1);

    if (abs(mean_gy) <= giro_deadzone) ready++;
    else gy_offset = gy_offset - mean_gy / (giro_deadzone + 1);

    if (abs(mean_gz) <= giro_deadzone) ready++;
    else gz_offset = gz_offset - mean_gz / (giro_deadzone + 1);

    if (ready == 6) break;
  }
}
