bool colore(int color) {
  uint16_t clear, red, green, blue;
  
  tcs.setInterrupt(false);
  
  delay(60);
  
  tcs.getRawData(&red, &green, &blue, &clear);
  tcs.setInterrupt(true);

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
    default:
      return false;
      break;
  }
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

//-------------------------------------------------------------------------------

float SRF10(byte address) {
  int reading = 0;
  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(address); // transmit to device #112 (0x70)
  // the address specified in the datasheet is 224 (0xE0)
  // but i2c adressing uses the high 7 bits so it's 112
  Wire.write(byte(0x00));      // sets register pointer to the command register (0x00)
  Wire.write(byte(0x51));      // command sensor to measure in "inches" (0x50)
  // use 0x51 for centimeters
  // use 0x52 for ping microseconds
  Wire.endTransmission();      // stop transmitting

  // step 2: wait for readings to happen
  delay(70);                   // datasheet suggests at least 65 milliseconds

  // step 3: instruct sensor to return a particular echo reading
  Wire.beginTransmission(address); // transmit to device #112
  Wire.write(byte(0x02));      // sets register pointer to echo #1 register (0x02)
  Wire.endTransmission();      // stop transmitting

  // step 4: request reading from sensor
  Wire.requestFrom(address, 2);    // request 2 bytes from slave device #112

  // step 5: receive reading from sensor
  if (2 <= Wire.available()) { // if two bytes were received
    reading = Wire.read();  // receive high byte (overwrites previous reading)
    reading = reading << 8;    // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
    return reading;
  }
}
