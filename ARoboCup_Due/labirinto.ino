void percorso(int x, int y, int livello) {
  bool prop[16];
  for (int i = 0; i < 16; i++) {
    prop[i] = proprieta(i, x, y, livello);
  }

  if (prop[5] == true)
    vittima(x, y, livello, 0);
  else if (prop[6] == true)
    vittima(x, y, livello, 1);
  else if (prop[7] == true)
    vittima(x, y, livello, 2);
  else if (prop[8] == true)
    vittima(x, y, livello, 3);

  int direzione = scegliDirezione(x, y, livello);
  muovi(direzione);
}

//-------------------------------------------------------------------------------

int scegliDirezione(int x, int y, int livello) {
  bool prop[16];
  for (int i = 0; i < 16; i++) {
    prop[i] = proprieta(i, x, y, livello);
  }
  int direzione = gyroscope(0, false);

  switch (direzione) {
    case 0:
      if (prop[1] == false && proprieta(0, x + 1, y, livello) == false)
        return 0;
      else if (prop[2] == false && proprieta(0, x, y + 1, livello) == false)
        return 1;
      else if (prop[4] == false && proprieta(0, x - 1, y, livello) == false)
        return 3;
      else if (prop[3] == false && proprieta(0, x, y - 1, livello) == false)
        return 2;
      else if (prop[3] == false)
        return 2;
      break;
    case 1:
      destra();
      break;
    case 2:
      dietro();
      break;
    case 3:
      sinistra();
      break;
  }
}
