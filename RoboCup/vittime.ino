void vittima(int x, int y, int livello, int posizione) {
  for (int i = 0; i < 5; i++) {
    digitalWrite(colorLED, HIGH);
    aspetta(500);
    digitalWrite(colorLED, LOW);
    aspetta(500);
  }

  if (kitCounter > 0)
    kitSalvataggio(posizione);

  aggiungiProprieta(x, y, livello, pVittimeSalvate);

  analogWrite(colorLED, powerColorLED);
}

//-------------------------------------------------------------------------------

void kitSalvataggio(int posizione) {
  if (posizione == 0)
    ruota(2);
  else if (posizione == 1)
    ruota(3);
  else if (posizione == 2)
    ruota(0);
  else if (posizione == 3)
    ruota (1);

  if (kitPosition)
    while (!servoDispenserFunction(posSXSM2)) {}
  else
    while (!servoDispenserFunction(posDXSM2)) {}

  aspetta(500);

  while (!servoDispenserFunction(posAVSM2)) {}

  kitPosition = !kitPosition;
  kitCounter--;
}