void seriale() {
  yield();
}

//-------------------------------------------------------------------------------

void aspetta(int tempoPausa) {
  int tempoIniziale = millis();
  while (millis() - tempoIniziale <= tempoPausa) {}
}
