void seriale() {
  Serial.println("dx");
  destra();
  aspetta(2000);
  Serial.println("sx");
  sinistra();
  aspetta(3000);
  /*avanzamento(30, 175);
    aspetta(2000);
    avanzamento(-30, 175);
    aspetta(2000);*/

  //Serial.println(sensoreDistanza(0));
  //Serial.println(gyroscope(2, true));
  //Serial.print(temperatura(-1, true));
  //Serial.print("   ");
  //Serial.println(temperatura(1, true));
  //kitSalvataggio(0);
  //delay(1000);

  yield();
}

//-------------------------------------------------------------------------------

void aspetta(unsigned int tempoPausa) {
  unsigned int tempoIniziale = millis();
  while (millis() - tempoIniziale <= tempoPausa) {}
}

//-------------------------------------------------------------------------------

void go_on() {
  while (digitalRead(button)) {}
  delay(300);
  Serial.println("go_on() Completed!");
}
