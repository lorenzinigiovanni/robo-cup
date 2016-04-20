bool colore(int color) {
  uint16_t clear, red, green, blue;

  tcs.getRawData(&red, &green, &blue, &clear);

  uint32_t sum = clear;
  float r, g, b;

  r = red * Kred;
  g = green * Kgreen;
  b = blue * Kblue;

  switch (color) {
    case 0:
      if (r > 200 && g > 200 && b > 200)
        return true;
    case 1:
      if (r < 50 && g < 50 && b < 50)
        return true;
    case 2:
      if (r > 100 && r < 200 && g > 100 && g < 200 && b > 100 && b < 200)
        return true;
    default:
      return false;
  }
  return false;
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
  Serial1.println("g");

  long unsigned int tempo = millis();
  while (!Serial1.available()) {
    if (millis() - tempo > gyroscopeTimeOut)
      break;
  }

  String str = Serial1.readStringUntil(10);

  for (int i = 0; i < 6; i++) {
    gyroArray[i] = str.substring(0, str.indexOf(',')).toFloat();
    str.remove(0, str.indexOf(',') + 1);
  }

  if (!rotazioneContinua)
    return gyroArray[scelta];
  else
    return gyroArray[scelta + 3];
}

//-------------------------------------------------------------------------------

float SRF10(byte address) {
  int reading = 0;

  Wire.beginTransmission(address);
  Wire.write(byte(0x00));
  Wire.write(byte(0x51));
  Wire.endTransmission();

  aspetta(70);

  Wire.beginTransmission(address);
  Wire.write(byte(0x02));
  Wire.endTransmission();

  Wire.requestFrom(int(address), 2);

  if (2 <= Wire.available()) {
    reading = Wire.read();
    reading = reading << 8;
    reading |= Wire.read();
  }

  return reading;
}

//-------------------------------------------------------------------------------

float MLX90614(bool sensor, bool measure) {
  uint16_t tempUK;
  uint8_t hB, lB, pec;
  byte sensorAddress = 0x5A;
  byte memoryAddress = 0x06;

  if (measure)
    memoryAddress = 0x07;

  if (sensor)
    sensorAddress = 0x55;

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
