bool colore(int color) {
  analogWrite(colorLED, powerColorLED);

  uint16_t clear, red, green, blue;

  tcs.getRawData(&red, &green, &blue, &clear);

  analogWrite(colorLED, 0);

  uint32_t sum = clear;
  float r, g, b;

  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;

  switch (color) {
    case 0:
      if (r > 210 && g > 210 && b > 210)
        return true;
    case 1:
      if (r < 10 && g < 10 && b < 10)
        return true;
    case 2:
      if (r > 180 && r < 200 && g > 180 && g < 200 && b > 180 && b < 200)
        return true;
    default:
      return false;
  }
}

//-------------------------------------------------------------------------------

float sensoreTemperatura(int numeroSensore) {
  float misure[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  float misura = 0;
  float massimo = 0;
  float minimo = 100000;
  bool sensor = false;

  if (numeroSensore == -1)
    sensor = false;
  if (numeroSensore == 1)
    sensor = true;

  for (int i = 0; i < 5; i++) {
    misure[i] += MLX90614(sensor, true);
    misure[i] -= MLX90614(sensor, false);
    massimo = max(massimo, misure[i]);
    minimo = min(minimo, misure[i]);
  }

  for (int i = 0; i < 5; i++)
    misura += misure[i];

  misura -= massimo;
  misura -= minimo;
  misura /= 3;
  return misura;
}

//-------------------------------------------------------------------------------

float sensoreDistanza(int numeroSensore) {
  float misure[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  float misura = 0;
  float massimo = 0;
  float minimo = 100000;
  int reading = 0;
  int indirizzo = 113;
  indirizzo += numeroSensore;

  for (int i = 0; i < 5; i++) {
    misure[i] = SRF10(indirizzo);
    massimo = max(massimo, misure[i]);
    minimo = min(minimo, misure[i]);
  }

  for (int i = 0; i < 5; i++)
    misura += misure[i];

  misura -= massimo;
  misura -= minimo;
  misura /= 3;
  return misura;
}

//-------------------------------------------------------------------------------

float gyroscope(int scelta, bool rotazioneContinua) {
  float misura = 0;

  while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  mpu.getFIFOBytes(fifoBuffer, packetSize);
  fifoCount -= packetSize;

  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  misura = ypr[scelta];
  misura = misura * 180 / M_PI;

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
  else {
    if (misura < errorePosizioni && misura > -errorePosizioni)
      return 0;
    else if (misura < 90 + errorePosizioni && misura > 90 - errorePosizioni)
      return 1;
    else if (misura < -180 + errorePosizioni || misura > 180 - errorePosizioni)
      return 2;
    else if (misura < -90 + errorePosizioni && misura > -90 - errorePosizioni)
      return 3;
  }
}

//-------------------------------------------------------------------------------

float SRF10(byte address) {
  int reading = 0;

  Wire.beginTransmission(address);
  Wire.write(byte(0x00));
  Wire.write(byte(0x51));
  Wire.endTransmission();

  delay(70);

  Wire.beginTransmission(address);
  Wire.write(byte(0x02));
  Wire.endTransmission();

  Wire.requestFrom(address, 2);

  if (2 <= Wire.available()) {
    reading = Wire.read();
    reading = reading << 8;
    reading |= Wire.read();
    return reading;
  }
}

//-------------------------------------------------------------------------------

float MLX90614(bool sensor, bool measure) {
  uint16_t tempUK;
  uint8_t hB, lB, pec;
  byte sensorAddress = 0x5A;
  byte memoryAddress;

  if (measure)
    memoryAddress = 0x07;
  else
    memoryAddress = 0x06;

  if (sensor)
    sensorAddress = 0x36;

  TWI_StartRead(pTwi, sensorAddress, memoryAddress, 1);

  lB = readByte();
  hB = readByte();

  TWI_SendSTOPCondition(pTwi);
  pec = readByte();

  while (!TWI_TransferComplete(pTwi))
    ;
  TWI_WaitTransferComplete(pTwi, RECV_TIMEOUT);

  tempUK = (hB << 8) | lB;
  if (tempUK & (1 << 16)) {
    return 0;
  }
  else {
    return ((float)tempUK * 2) / 100 ;
  }
}
