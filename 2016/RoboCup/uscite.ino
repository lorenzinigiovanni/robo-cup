bool servoTorrettaFunction(int gradi) {
  servoTorretta.write(gradi);
  if (posizioneSM1 != gradi) {
    aspetta(abs(gradi - posizioneSM1)*timeSM1);
    posizioneSM1 = gradi;
  }
  return true;
}

//-------------------------------------------------------------------------------

bool servoDispenserSXFunction(int gradi) {
  servoDispenserSX.write(gradi);
  if (posizioneSM2 != gradi) {
    aspetta(abs(gradi - posizioneSM2)*timeSM2);
    posizioneSM2 = gradi;
  }
  return true;
}

//-------------------------------------------------------------------------------

bool servoDispenserDXFunction(int gradi) {
  servoDispenserDX.write(gradi);
  if (posizioneSM3 != gradi) {
    aspetta(abs(gradi - posizioneSM3)*timeSM3);
    posizioneSM3 = gradi;
  }
  return true;
}

//-------------------------------------------------------------------------------

void motori(float powerM1, float powerM2) {
  powerM2 = constrain(powerM2, -255, 255);
  powerM1 = constrain(powerM1, -255, 255);

  if (powerM1 > 0) {
    analogWrite(M1F, powerM1);
    analogWrite(M1R, 0);
    digitalWrite(M1E, HIGH);
  }
  else if (powerM1 < 0) {
    analogWrite(M1F, 0);
    analogWrite(M1R, -powerM1);
    digitalWrite(M1E, HIGH);
  }
  else
    digitalWrite(M1E, LOW);

  if (powerM2 > 0) {
    analogWrite(M2F, powerM2);
    analogWrite(M2R, 0);
    digitalWrite(M2E, HIGH);
  }
  else if (powerM2 < 0) {
    analogWrite(M2F, 0);
    analogWrite(M2R, -powerM2);
    digitalWrite(M2E, HIGH);
  }
  else
    digitalWrite (M2E, LOW);
}

//-------------------------------------------------------------------------------

void resetArduinoNano() {
  digitalWrite(resetNano, LOW);
  aspetta(500);
  digitalWrite(resetNano, HIGH);
}

//-------------------------------------------------------------------------------

void light(int time, int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(colorLED, HIGH);
    delay(time);
    digitalWrite(colorLED, LOW);
    delay(time);
  }
  analogWrite(colorLED, powerColorLED);
}