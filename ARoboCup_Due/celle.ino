bool proprieta(int prop, int x, int y, int livello) {
  int matrice = 0;
  if (livello == 1)
    matrice = matriceLvl1[x][y];
  else if (livello == 2)
    matrice = matriceLvl2[x][y];

  for (int i = 15; i > -1; i--) {
    int powwa = pow(2, i);
    Serial.println(i);
    if (matrice - powwa >= 0) {
      matrice -= powwa;
      Serial.print("PROP= ") && Serial.print(prop) && Serial.print("\ti= ") && Serial.println(i);
      if (prop == i)
        return true;
    }
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

//-------------------------------------------------------------------------------

/*bool propieta(int prop, int x, int y, unsigned int matrix[sizeX][sizeY]) {
  if (matrix[x][y] >= 2048) {
    if (prop == 11)
      return true;
    else
      matrix[x][y] -= 2048;
  }
  else if (prop == 11)
    return false;

  //-------------------------

  if (matrix[x][y] >= 1024) {
    if (prop == 10)
      return true;
    else
      matrix[x][y] -= 1024;
  }
  else if (prop == 10)
    return false;

  //-------------------------

  if (matrix[x][y] >= 512) {
    if (prop == 9)
      return true;
    else
      matrix[x][y] -= 512;
  }
  else if (prop == 9)
    return false;

  //-------------------------

  if (matrix[x][y] >= 256) {
    if (prop == 8)
      return true;
    else
      matrix[x][y] -= 256;
  }
  else if (prop == 8)
    return false;

  //-------------------------

  if (matrix[x][y] >= 128) {
    if (prop == 7)
      return true;
    else
      matrix[x][y] -= 128;
  }
  else if (prop == 7)
    return false;

  //-------------------------

  if (matrix[x][y] >= 64) {
    if (prop == 6)
      return true;
    else
      matrix[x][y] -= 64;
  }
  else if (prop == 6)
    return false;

  //-------------------------

  if (matrix[x][y] >= 32) {
    if (prop == 5)
      return true;
    else
      matrix[x][y] -= 32;
  }
  else if (prop == 5)
    return false;

  //-------------------------

  if (matrix[x][y] >= 16) {
    if (prop == 4)
      return true;
    else
      matrix[x][y] -= 16;
  }
  else if (prop == 4)
    return false;

  //-------------------------

  if (matrix[x][y] >= 8) {
    if (prop == 3)
      return true;
    else
      matrix[x][y] -= 8;
  }
  else if (prop == 3)
    return false;

  //-------------------------

  if (matrix[x][y] >= 4) {
    if (prop == 2)
      return true;
    else
      matrix[x][y] -= 4;
  }
  else if (prop == 2)
    return false;

  //-------------------------

  if (matrix[x][y] >= 2) {
    if (prop == 1)
      return true;
    else
      matrix[x][y] -= 2;
  }
  else if (prop == 1)
    return false;

  //-------------------------

  if (matrix[x][y] >= 1) {
    if (prop == 0)
      return true;
  }
  else if (prop == 0)
    return false;
  }*/

