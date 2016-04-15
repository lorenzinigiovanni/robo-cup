void seriale() {
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
