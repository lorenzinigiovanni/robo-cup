void sensorSetup() {
  sensorEnabler();
  initGiroscopio();
  initColor();
  //initTermometro();
  initDistanziometro();
}

//-------------------------------------------------------------------------------

void sensorCalibration() {
  //calGiroscopio(100, 8, 1);
}

//-------------------------------------------------------------------------------

void pinSetup() {
  pinMode(M1E, OUTPUT);
  pinMode(M1F, OUTPUT);
  pinMode(M1R, OUTPUT);
  pinMode(M2F, OUTPUT);
  pinMode(M2R, OUTPUT);
  pinMode(M2E, OUTPUT);

  pinMode(LED, OUTPUT);
  pinMode(ENGYRO, OUTPUT);
  pinMode(ENRGB, OUTPUT);
  pinMode(ENTMP1, OUTPUT);
  pinMode(ENTMP2, OUTPUT);

  pinMode(colorLED, OUTPUT);
  analogWrite(colorLED, 255);

  servoTorretta.attach(SM1);
  pixels.begin();

  Wire.begin();
  //lcd.begin();
}

//-------------------------------------------------------------------------------

void sensorEnabler() {
  delay(500);
  digitalWrite(ENGYRO, HIGH);
  digitalWrite(ENRGB, HIGH);
  digitalWrite(ENTMP1, HIGH);
  digitalWrite(ENTMP2, HIGH);
  delay(500);
}

//-------------------------------------------------------------------------------

void initGiroscopio() {
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  mpu.initialize();
  digitalWrite(LED, LOW);
  digitalWrite(LED, mpu.testConnection() ? HIGH : LOW);

  if (!mpu.testConnection()) {
    digitalWrite(ENGYRO, LOW);
    sensorSetup();
  }

  devStatus = mpu.dmpInitialize();

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
}

//-------------------------------------------------------------------------------

void initColor() {
  if (!tcs.begin()) {
    digitalWrite(ENRGB, LOW);
    sensorSetup();
  }
}

//-------------------------------------------------------------------------------

void initTermometro() {
  Wire1_Init();
  pTwi->TWI_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;
  TWI_ConfigureMaster(pTwi, TWI_CLOCK, VARIANT_MCK);
}

//-------------------------------------------------------------------------------

void initDistanziometro() {
  setGain(112, 6);
  setGain(113, 6);
  setGain(114, 6);
  setRange(112, 255);
  setRange(113, 255);
  setRange(114, 255);
}

void setGain(byte address, byte range) {
  Wire.beginTransmission(address);
  Wire.write(byte(0x01));
  Wire.write(byte(range));
  Wire.endTransmission();
}

void setRange(byte address, byte range) {
  Wire.beginTransmission(address);
  Wire.write(byte(0x02));
  Wire.write(byte(range));
  Wire.endTransmission();
}

//-------------------------------------------------------------------------------

void calGiroscopio(int buffersize, int acel_deadzone, int giro_deadzone) {
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);

  if (state == 0) {
    calGiroscopioMeansensors(buffersize, acel_deadzone, giro_deadzone);
    state++;
  }

  if (state == 1) {
    calGiroscopioCalibration(buffersize, acel_deadzone, giro_deadzone);
    state++;
  }

  if (state == 2) {
    calGiroscopioMeansensors(buffersize, acel_deadzone, giro_deadzone);
  }
}

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
