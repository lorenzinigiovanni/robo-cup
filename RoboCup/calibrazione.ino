void sensorCalibration() {
  calibrazioneTermometro();
  calibrazioneColor();
}

//-------------------------------------------------------------------------------

void calibrazioneColor() {
  uint16_t clear, red, green, blue;

  tcs.getRawData(&red, &green, &blue, &clear);

  uint32_t sum = clear;
  float r, g, b;

  r = red;
  g = green;
  b = blue;

  Kred = 255 / r;
  Kgreen = 255 / g;
  Kblue = 255 / b;
}

//-------------------------------------------------------------------------------

void calibrazioneTermometro() {
  int t = 0;
  int n = 10;

  for (int i = 0; i < n; i++) {
    t += sensoreTemperatura(1);
    t += sensoreTemperatura(-1);
  }

  t /= (n * 2);
  sogliaTemperatura = t + differenzaTemperatura;
}
