void seriale() {
   /*muovi(0);
   aspetta(500);
   muovi(3);
   aspetta(500);
   muovi(1);*/
   Serial.println(gyroscope(0, true));
  yield();
}

//-------------------------------------------------------------------------------

void display() {
}

//-------------------------------------------------------------------------------

void aspetta(int tempoPausa) {
  int tempoIniziale = millis();
  while (millis() - tempoIniziale < tempoPausa) {}
}
