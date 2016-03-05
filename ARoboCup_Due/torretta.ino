float temperatura(int posizione, bool relativa) {
  int servo = 90;

  if (relativa) {
    switch (posizione) {
      case -1:
        servo = 0;
        break;
      case 0:
        servo = 90;
        break;
      case 1:
        servo = 180;
        break;
    }
  }

  else {
    posizione = posizione - gyroscope(0, false);
    if (posizione > 1 || posizione < -1)
      return -1;
    servo = posizione * 90 + 90;
  }

  while (!servoTorreta(servo)) {}

  float misura = sensoreTemperatura();
  return misura;
}

//-------------------------------------------------------------------------------

float distanza(int posizione, bool relativa) {
  int servo = 90;
  int numeroSensore = 0;

  if (relativa) {
    switch (posizione) {
      case -1:
        servo = 90;
        numeroSensore = -1;
        break;
      case 0:
        servo = 90;
        numeroSensore = 0;
        break;
      case 1:
        servo = 90;
        numeroSensore = 1;
        break;
      case 2:
        servo = 180;
        numeroSensore = 1;
        break;
    }
  }

  else {
    posizione = posizione - gyroscope(0, false);
    switch (posizione) {
      case -3:
        servo = 90;
        numeroSensore = 1;
        break;
      case -2:
        servo = 0;
        numeroSensore = -1;
        break;
      case -1:
        servo = 90;
        numeroSensore = -1;
        break;
      case 0:
        servo = 90;
        numeroSensore = 0;
        break;
      case 1:
        servo = 90;
        numeroSensore = 1;
        break;
      case 2:
        servo = 180;
        numeroSensore = 1;
        break;
      case 3:
        servo = 90;
        numeroSensore = -1;
        break;
    }
  }

  while (!servoTorreta(servo)) {}

  float misura = sensoreDistanza(numeroSensore);
  return misura;
}
