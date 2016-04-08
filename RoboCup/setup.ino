void sensorSetup() {
  sensorEnabler();
  initGiroscopio();
  initColor();
  initTermometro();
  initDistanziometro();
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

  pinMode(colorLED, OUTPUT);

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
  digitalWrite(ENTMP, HIGH);
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
  setGain(112, gain);
  setGain(113, gain);
  setGain(114, gain);
  setRange(112, range);
  setRange(113, range);
  setRange(114, range);
}

//-------------------------------------------------------------------------------

void setGain(byte address, byte range) {
  Wire.beginTransmission(address);
  Wire.write(byte(0x01));
  Wire.write(byte(range));
  Wire.endTransmission();
}

//-------------------------------------------------------------------------------

void setRange(byte address, byte range) {
  Wire.beginTransmission(address);
  Wire.write(byte(0x02));
  Wire.write(byte(range));
  Wire.endTransmission();
}
