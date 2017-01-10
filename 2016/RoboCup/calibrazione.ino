void sensorCalibration() {
  light(100, 3);
  go_on();
  calibrazioneTermometro();
  light(100, 3);
  go_on();
  calibrazioneColor();
}

//-------------------------------------------------------------------------------

void calibrazioneColor() {
  Serial.print("calibrazione colorimetro...");
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
  Serial.println("OK");
}

//-------------------------------------------------------------------------------

void calibrazioneTermometro() {
  Serial.print("calibrazione termometro...");
  int t = 0;
  int n = 10;

  for (int i = 0; i < n; i++) {
    t += sensoreTemperatura(1);
    t += sensoreTemperatura(-1);
  }

  t /= (n * 2);
  sogliaTemperatura = t + differenzaTemperatura;
  Serial.println("OK");
}
