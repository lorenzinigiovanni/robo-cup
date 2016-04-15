bool proprieta(int x, int y, int livello, int prop) {
  int var = 0;
  var = mappa[x][y][livello];

  for (int i = 15; i > -1; i--) {
    int powwa = pow(2, i);
    if (var - powwa >= 0) {
      var -= powwa;
      if (prop == i)
        return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------------------

bool aggiungiProprieta(int x, int y, int livello, int prop) {
  if (proprieta(x, y, livello, prop) == false) {
    prop = pow(2, prop);
    mappa[x][y][livello] += prop;
    return true;
  }
  return false;
}

//-------------------------------------------------------------------------------

void mappatura(int x, int y, int livello) {
  if (!proprieta(x, y, livello, pCellaVisitata)) {
    aggiungiProprieta(x, y, livello, pCellaVisitata);

    if (distanza(0, false) < 20) {
      aggiungiProprieta(x, y, livello, pMuroPosizione0);
      if (temperatura(0, false) > sogliaTemperatura)
        aggiungiProprieta(x, y, livello, pVittimaPosizione0);
    }
    if (distanza(1, false) < 20) {
      aggiungiProprieta(x, y, livello, pMuroPosizione1);
      if (temperatura(1, false) > sogliaTemperatura)
        aggiungiProprieta(x, y, livello, pVittimaPosizione1);
    }
    if (distanza(2, false) < 20) {
      aggiungiProprieta(x, y, livello, pMuroPosizione2);
      if (temperatura(2, false) > sogliaTemperatura)
        aggiungiProprieta(x, y, livello, pVittimaPosizione2);
    }
    if (distanza(3, false) < 20) {
      aggiungiProprieta(x, y, livello, pMuroPosizione3);
      if (temperatura(3, false) > sogliaTemperatura)
        aggiungiProprieta(x, y, livello, pVittimaPosizione3);
    }
    if (colore(1))
      aggiungiProprieta(x, y, livello, pCellaNera);
    else if (colore(2))
      aggiungiProprieta(x, y, livello, pCellaGrigia);
  }
}
