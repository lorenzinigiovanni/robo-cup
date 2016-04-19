void seriale() {
  /*Serial.print("distanza 0: ");
  Serial.println(distanza(0, true));
  Serial.print("giroscopio 0: ");
  Serial.println(gyroscope(0, true));*/
  ruota(-1);
  aspetta(5000);
  ruota(3);
  yield();
}

//-------------------------------------------------------------------------------

void aspetta(unsigned int tempoPausa) {
  unsigned int tempoIniziale = millis();
  while (millis() - tempoIniziale <= tempoPausa) {}
}
