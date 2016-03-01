bool colore(int color) {
  uint16_t clear, red, green, blue;

  tcs.setInterrupt(false);

  delay(60);

  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);

  //----------------DEBUG-------------------
  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);

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
      break;
    case 1:
      if (r < 10 && g < 10 && b < 10)
        return true;
      break;
    case 2:
      if (r > 180 && r < 200 && g > 180 && g < 200 && b > 180 && b < 200)
        return true;
      break;
  }
  return false;
}

//-------------------------------------------------------------------------------

float sensoreTemperatura() {
  float misure[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  float misura = 0;
  float massimo = 0;
  float minimo = 1000;

  for (int i = 0; i < 5; i++) {
    if (therm.read()) {
      misure[i] = therm.object();
      misure[i] -= therm.ambient();
      massimo = max(massimo, misure[i]);
      minimo = min(minimo, misure[i]);
    }
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
  float minimo = 1000;
  int reading = 0;
  int indirizzo = 0;

  switch (numeroSensore) {
    case -1:
      indirizzo = 112;
      break;
    case 0:
      indirizzo = 113;
      break;
    case 1:
      indirizzo = 114;
      break;
  }

  for (int i = 0; i < 5; i++) {
    Wire.beginTransmission(indirizzo);
    Wire.write(byte(0x00));
    Wire.write(byte(0x51));
    Wire.endTransmission();

    Wire.beginTransmission(indirizzo);
    Wire.write(byte(0x02));
    Wire.endTransmission();
    Wire.requestFrom(indirizzo, 2);

    if (2 <= Wire.available()) {
      reading = Wire.read();
      reading = reading << 8;
      reading |= Wire.read();
      Serial.println(reading);
    }
    misure[i] = reading;
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
    if (misura < 10 && misura > -10)
      return 0;
    else if (misura < 100 && misura > 80)
      return 1;
    else if (misura < -170 || misura > 170)
      return 2;
    else if (misura < -80 && misura > -100)
      return 3;
  }
}
