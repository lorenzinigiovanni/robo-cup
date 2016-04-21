void seriale() {
  //Serial.print("giroscopio 0: ");
  //Serial.println(gyroscope(0, true));
  /*avanzamento(-30, 150);
  rotazione(-90, 150);
  aspetta(500);
  muovi(0);
  aspetta(1000);
  muovi(1);
  aspetta(1000);
  muovi(0);
  aspetta(1000);
  //colore(0);

  Serial.print("TMP[1]: ");
  Serial.print(sensoreTemperatura(1));
  Serial.print(" TMP[-1]: ");
  Serial.println(sensoreTemperatura(-1));*/

  rotazione(-90, 200);
  aspetta(2000);
  rotazione(90, 200);
  aspetta(2000);
  
  yield();
}

//-------------------------------------------------------------------------------

void aspetta(unsigned int tempoPausa) {
  unsigned int tempoIniziale = millis();
  while (millis() - tempoIniziale <= tempoPausa) {}
}
