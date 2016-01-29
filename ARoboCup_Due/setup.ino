void settup() {
  pin();

  Serial.begin(115200);

  delay(500);

  initGiroscopio();
  /*
    initColor();

    initTermometro();*/
}

void pin() {
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

  digitalWrite(ENGYRO, HIGH);
  digitalWrite(ENRGB, HIGH);
  digitalWrite(ENTMP, HIGH);

  servoTorretta.attach(SM1);
  pixels.begin();
}

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
}

void initColor() {
  if (!tcs.begin()) {
    digitalWrite(ENRGB, LOW);
    setup();
  }
}

void initTermometro() {
  if (!therm.begin()) {
    digitalWrite(ENTMP, LOW);
    setup();
  }

  therm.setUnit(TEMP_C);
}
