bool proprieta(int prop, int x, int y, int livello) {
  int matrice = 0;
  if (livello == 1)
    matrice = matriceLvl1[x][y];
  else if (livello == 2)
    matrice = matriceLvl2[x][y];

  for (int i = 15; i > -1; i--) {
    int powwa = pow(2, i);
    Serial.println(matrice);
    if (matrice - powwa >= 0) {
      matrice -= powwa;
      if (prop == i)
        return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------------------

bool aggiungiProprieta(int x, int y, int livello, int prop) {
  if (proprieta(prop, x, y, livello) == false) {
    prop = pow(2, prop);
    if (livello == 1)
      matriceLvl1[x][y] += prop;
    else if (livello == 2)
      matriceLvl2[x][y] += prop;
    return true;
  }
  return false;
}

//-------------------------------------------------------------------------------

void mappatura(int x, int y, int livello) {
  int prop = 0;
  if (livello == 1)
    prop = matriceLvl1[x][y];
  else if (livello == 2)
    prop = matriceLvl2[x][y];

  if (prop == 0) {
    prop += 1;

    if (distanza(0, false) < 20) {
      prop += 2;
      if (temperatura(0, false) > 8)
        prop += 32;
    }
    if (distanza(1, false) < 20) {
      prop += 4;
      if (temperatura(1, false) > 8)
        prop += 64;
    }
    if (distanza(2, false) < 20) {
      prop += 8;
      if (temperatura(2, false) > 8)
        prop += 128;
    }
    if (distanza(3, false) < 20) {
      prop += 16;
      if (temperatura(3, false) > 8)
        prop += 256;
    }

    if (colore(1))
      prop += 1024;
    if (colore(2))
      prop += 2048;
  }

  if (livello == 1)
    matriceLvl1[x][y] = prop;
  else if (livello == 2)
    matriceLvl2[x][y] = prop;
}
