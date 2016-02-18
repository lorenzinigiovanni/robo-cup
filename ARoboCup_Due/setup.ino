void sensorSetup() {
  sensorEnabler();
  initGiroscopio();
  //initColor();
  //initTermometro();
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
  pinMode(ENTMP, OUTPUT);

  servoTorretta.attach(SM1);
  pixels.begin();
  
  Wire.begin();
  lcd.begin();
}

//-------------------------------------------------------------------------------

void sensorEnabler() {
  delay(500);
  digitalWrite(ENGYRO, HIGH);
  digitalWrite(ENRGB, HIGH);
  digitalWrite(ENTMP, HIGH);
  delay(500);
}

//-------------------------------------------------------------------------------

void initGiroscopio() {
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  //  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz). Comment this line if having compilation difficulties with TWBR.
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
//-2678  -1053 1165  34  -42 65

  devStatus = mpu.dmpInitialize();
  mpu.setXAccelOffset(-2678);
  mpu.setYAccelOffset(-1053);
  mpu.setZAccelOffset(1165);
  mpu.setXGyroOffset(34);
  mpu.setYGyroOffset(-42);
  mpu.setZGyroOffset(65);

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
  if (!therm.begin()) {
    digitalWrite(ENTMP, LOW);
    sensorSetup();
  }

  therm.setUnit(TEMP_C);
}
