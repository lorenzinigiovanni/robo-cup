void seriale() {
  /*Serial.println("90, 100");
    rotazione(90, 100);
    aspetta(2000);
    Serial.println("-90, 100");
    rotazione(-90, 100);
    aspetta(5000);*/
  /*avanzamento(30, 175);
    aspetta(2000);
    avanzamento(-30, 175);
    aspetta(2000);*/

  Serial.println(sensoreDistanza(0));
  //Serial.println(gyroscope(2, true));
  
  yield();
}

//-------------------------------------------------------------------------------

void aspetta(unsigned int tempoPausa) {
  unsigned int tempoIniziale = millis();
  while (millis() - tempoIniziale <= tempoPausa) {}
}
