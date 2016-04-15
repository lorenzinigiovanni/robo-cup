bool servoTorreta(int gradi) {
  servoTorretta.write(gradi);
<<<<<<< HEAD
  if (posizioneSM1 != gradi) {
    aspetta(abs(gradi - posizioneSM1)*timeSM1);
    posizioneSM1 = gradi;
  }
=======
  delay(1000);
>>>>>>> fb7d1296f6f78e3d74fb1768bbb5a8f1b37602ea
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

void led(int R, int G, int B, int ritardo) {
  for (int i = 0; i < 3; i++) {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
    pixels.show();
  }
  if (ritardo > 0) {
    aspetta(ritardo);
    for (int i = 0; i < 3; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();
    }
  }
}
