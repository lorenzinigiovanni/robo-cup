void vittima(int x, int y, int livello, int posizione) {
  for (int i = 0; i < 3; i++) {
    beep.write(180);
    digitalWrite(colorLED, HIGH);
    aspetta(500);
    beep.write(0);
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
    ruota(1);

  if (kitPosition) {
    while (!servoDispenserSXFunction(posDownSM2)) {}
    aspetta(500);
    while (!servoDispenserSXFunction(posOpenSM2)) {}
    aspetta(500);
    while (!servoDispenserSXFunction(posCloseSM2)) {}
  }
  else {
    while (!servoDispenserDXFunction(posDownSM3)) {}
    aspetta(500);
    while (!servoDispenserDXFunction(posOpenSM3)) {}
    aspetta(500);
    while (!servoDispenserDXFunction(posCloseSM3)) {}
  }

  kitPosition = !kitPosition;
  kitCounter--;
}
