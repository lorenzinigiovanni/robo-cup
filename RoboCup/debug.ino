void seriale() {
  /*avanti();
  delay(1000);
  dietro();
  delay(1000);*/
  //Serial.println(gyroscope(1, true));
  //Serial.println(distanza(0, true));
  //colore(0);
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
