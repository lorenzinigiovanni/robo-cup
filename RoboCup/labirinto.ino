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
  if (!proprieta(x, y, livello, pMuroPosizione0)) {
    if (!proprieta(x + 1, y, livello, pCellaVisitata))
      return 0;
  }
  else if (!proprieta(x, y, livello, pMuroPosizione1)) {
    if (!proprieta(x, y + 1, livello, pCellaVisitata))
      return 1;
  }
  else if (!proprieta(x, y, livello, pMuroPosizione3)) {
    if (!proprieta(x, y - 1, livello, pCellaVisitata))
      return 3;
  }
  else if (!proprieta(x, y, livello, pMuroPosizione2)) {
    if (!proprieta(x - 1, y, livello, pCellaVisitata))
      return 2;
  }
  else if (!proprieta(x, y, livello, pMuroPosizione0)) {
    if (!proprieta(x + 1, y, livello, pCellaVisitata))
      return 0;
  }
  else if (!proprieta(x, y, livello, pMuroPosizione1)) {
    if (!proprieta(x, y + 1, livello, pCellaVisitata))
      return 1;
  }
  else if (!proprieta(x, y, livello, pMuroPosizione3)) {
    if (!proprieta(x, y - 1, livello, pCellaVisitata))
      return 3;
  }
  else if (!proprieta(x, y, livello, pMuroPosizione2)) {
    if (!proprieta(x - 1, y, livello, pCellaVisitata))
      return 2;
  }
}
