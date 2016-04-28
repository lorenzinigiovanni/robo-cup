void sensorSetup() {
  sensorEnabler();
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
  pinMode(ENRGB, OUTPUT);
  pinMode(ENTMP, OUTPUT);

  pinMode(colorLED, OUTPUT);

  pinMode(resetNano, OUTPUT);
  digitalWrite(resetNano, HIGH);

  pinMode(button, INPUT_PULLUP);

  servoTorretta.attach(SM1);
  servoDispenserSX.attach(SM2);
  servoDispenserDX.attach(SM3);
  beep.attach(9);
  while (!servoTorrettaFunction(posAVSM1)) {}
  while (!servoDispenserSXFunction(posCloseSM2)) {}
  while (!servoDispenserDXFunction(posCloseSM3)) {}

  Wire.begin();
}

//-------------------------------------------------------------------------------

void sensorEnabler() {
  aspetta(500);
  digitalWrite(ENRGB, HIGH);
  digitalWrite(ENTMP, HIGH);
  aspetta(500);
}

//-------------------------------------------------------------------------------

void initColor() {
  if (!tcs.begin()) {
    digitalWrite(ENRGB, LOW);
    sensorSetup();
  }
  analogWrite(colorLED, powerColorLED);
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
