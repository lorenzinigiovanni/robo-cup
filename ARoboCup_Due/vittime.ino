void vittima(int x, int y, int livello, int posizione) {
  led(255, 0, 0, 5000);
  if (kitCounter > 0)
    kitSalvataggio(posizione);
  aggiungiProprieta(x, y, livello, 9);
}

void kitSalvataggio(int posizione) {
  kitCounter--;
}
