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

//const unsigned int minPassage = 3;

int scegliDirezione(int x, int y, int livello) {
  int a = 999, b = 999, c = 999, d = 999;
  if (!proprieta(x, y, livello, pMuroPosizione0) && !proprieta(x + 1, y, livello, pCellaNera))
    a = passaggi[x + 1][y][livello];
  if (!proprieta(x, y, livello, pMuroPosizione1) && !proprieta(x, y + 1, livello, pCellaNera))
    b = passaggi[x][y + 1][livello];
  if (!proprieta(x, y, livello, pMuroPosizione2) && !proprieta(x - 1, y, livello, pCellaNera))
    c = passaggi[x - 1][y][livello];
  if (!proprieta(x, y, livello, pMuroPosizione3) && !proprieta(x, y - 1, livello, pCellaNera))
    d = passaggi[x][y - 1][livello];

  return posMin(a, b, c, d);
}

/*if (!proprieta(x, y, livello, pCellaNera))
  return -99; //indietro boh
  if (proprieta(x, y, livello, pMuroPosizione0))
  if (!proprieta(x, y, livello, pMuroPosizione1) && passaggi[x][y + 1][livello] < minPassage)
    return 1;
  else if (!proprieta(x, y, livello, pMuroPosizione3) && passaggi[x][y - 1][livello] < minPassage)
    return 3;
  else
    return 2; //probabile killer loop
  else if (proprieta(x, y, livello, pMuroPosizione1))
  if (!proprieta(x, y, livello, pMuroPosizione0) && passaggi[x + 1][y][livello] < minPassage)
    return 0;
  else if (!proprieta(x, y, livello, pMuroPosizione2) && passaggi[x - 1][y][livello] < minPassage)
    return 2;
  else
    return 3; //probabile killer loop
  else if (proprieta(x, y, livello, pMuroPosizione3))
  if (!proprieta(x, y, livello, pMuroPosizione0) && passaggi[x + 1][y][livello] < minPassage) //forse separare in due if
    return 0;
  else if (!proprieta(x, y, livello, pMuroPosizione2) && passaggi[x - 1][y][livello] < minPassage)
    return 2;
  else
    return 1; //probabile killer loop
  else if (proprieta(x, y, livello, pMuroPosizione2))
  if (!proprieta(x, y, livello, pMuroPosizione1) && passaggi[x][y + 1][livello] < minPassage)
    return 1;
  else if (!proprieta(x, y, livello, pMuroPosizione3) && passaggi[x][y - 1][livello] < minPassage)
    return 3;
  else
    return 0; //probabile killer loop */

//-------------------------------------------------------------------------------

int posMin(int a, int b, int c, int d) {
  if (a <= b && a <= c && a <= d)
    return 0;
  else if (b <= a && b <= c && b <= d)
    return 1;
  else if (d <= a && d <= b && d <= c)
    return 3;
  else if (c <= a && c <= b && c <= d)
    return 2;
}
