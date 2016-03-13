float temperatura(int posizione, bool relativa) {
  int servo = 90;
  int numeroSensore = 0;

  if (!relativa)
    posizione -= gyroscope(0, false);

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
      servo = 180;
      numeroSensore = -1;
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

  while (!servoTorreta(servo)) {}

  float misura = sensoreTemperatura(numeroSensore);
  return misura;
}

//-------------------------------------------------------------------------------

float distanza(int posizione, bool relativa) {
  int servo = 90;
  int numeroSensore = 0;

  if (!relativa)
    posizione -= gyroscope(0, false);

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

  while (!servoTorreta(servo)) {}

  float misura = sensoreDistanza(numeroSensore);
  return misura;
}
