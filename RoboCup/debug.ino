void seriale() {
  Serial.println("90, 100");
  avanti();
  aspetta(2000);
  Serial.println("-90, 100");
  dietro();
  aspetta(5000);
  /*avanzamento(30, 175);
    aspetta(2000);
    avanzamento(-30, 175);
    aspetta(2000);*/

  //Serial.println(sensoreDistanza(0));
  //Serial.println(gyroscope(2, true));

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
