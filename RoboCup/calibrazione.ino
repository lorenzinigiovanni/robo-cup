void sensorCalibration() {
  calibrazioneTermometro();
}

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
