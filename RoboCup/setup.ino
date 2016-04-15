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

  servoTorretta.attach(SM1);
  pixels.begin();

  Wire.begin();
  //lcd.begin();
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
