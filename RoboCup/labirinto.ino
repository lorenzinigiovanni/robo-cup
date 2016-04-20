void percorso(int x, int y, int livello) {
  if (proprieta(x, y, livello, pVittimaPosizione0))
    vittima(x, y, livello, 0);
  if (proprieta(x, y, livello, pVittimaPosizione1))
    vittima(x, y, livello, 1);
  if (proprieta(x, y, livello, pVittimaPosizione2))
    vittima(x, y, livello, 2);
  if (proprieta(x, y, livello, pVittimaPosizione3))
    vittima(x, y, livello, 3);

  int direzione = scegliDirezione(x, y, livello);
  previousX = x;
  previousY = y;
  previousL = livello;
  muovi(direzione);
}

//-------------------------------------------------------------------------------

int scegliDirezione(int x, int y, int livello) {
  return x / y * livello;
}
