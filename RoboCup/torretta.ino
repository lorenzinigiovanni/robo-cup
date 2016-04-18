float temperatura(int posizione, bool relativa) {
  int servo = posAVSM1;
  int numeroSensore = 0;

  if (!relativa)
    posizione -= gyroscope(0, false);

  switch (posizione) {
    case -3:
      servo = posAVSM1;
      numeroSensore = 1;
      break;
    case -2:
      if (posizioneSM1 == posSXSM1) {
        servo = posSXSM1;
        numeroSensore = -1;
      } else {
        servo = posDXSM1;
        numeroSensore = 1;
      }
      break;
    case -1:
      servo = posAVSM1;
      numeroSensore = -1;
      break;
    case 0:
      if (posizioneSM1 == posDXSM1) {
        servo = posDXSM1;
        numeroSensore = -1;
      } else {
        servo = posSXSM1;
        numeroSensore = 1;
      }
      break;
    case 1:
      servo = posAVSM1;
      numeroSensore = 1;
      break;
    case 2:
      if (posizioneSM1 == posSXSM1) {
        servo = posSXSM1;
        numeroSensore = -1;
      } else {
        servo = posDXSM1;
        numeroSensore = 1;
      }
      break;
    case 3:
      servo = posAVSM1;
      numeroSensore = -1;
      break;
  }

  while (!servoTorrettaFunction(servo)) {}

  float misura = sensoreTemperatura(numeroSensore);
  return misura;
}

//-------------------------------------------------------------------------------

float distanza(int posizione, bool relativa) {
  int servo = posAVSM1;
  int numeroSensore = 0;

  if (!relativa)
    posizione -= gyroscope(0, false);

  switch (posizione) {
    case -3:
      servo = posAVSM1;
      numeroSensore = 1;
      break;
    case -2:
      if (posizioneSM1 == posSXSM1) {
        servo = posSXSM1;
        numeroSensore = -1;
      } else {
        servo = posDXSM1;
        numeroSensore = 1;
      }
      break;
    case -1:
      servo = posAVSM1;
      numeroSensore = -1;
      break;
    case 0:
      servo = posAVSM1;
      numeroSensore = 0;
      break;
    case 1:
      servo = posAVSM1;
      numeroSensore = 1;
      break;
    case 2:
      if (posizioneSM1 == posSXSM1) {
        servo = posSXSM1;
        numeroSensore = -1;
      } else {
        servo = posDXSM1;
        numeroSensore = 1;
      }
      break;
    case 3:
      servo = posAVSM1;
      numeroSensore = -1;
      break;
  }

  while (!servoTorrettaFunction(servo)) {}

  float misura = sensoreDistanza(numeroSensore);
  return misura;
}
