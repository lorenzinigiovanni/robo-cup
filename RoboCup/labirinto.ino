void percorso(int x, int y, int livello) {
  bool prop[16];
  for (int i = 0; i < 16; i++)
    prop[i] = proprieta(x, y, livello, i);

  if (prop[5])
    vittima(x, y, livello, 0);
  if (prop[6])
    vittima(x, y, livello, 1);
  if (prop[7])
    vittima(x, y, livello, 2);
  if (prop[8])
    vittima(x, y, livello, 3);

  int direzione = scegliDirezione(x, y, livello);
  muovi(direzione);
}

//-------------------------------------------------------------------------------

int scegliDirezione(int x, int y, int livello) {
  bool prop[16];
  for (int i = 0; i < 16; i++) {
    prop[i] = proprieta(x, y, livello, i);
  }
  int direzione = gyroscope(0, false);

  switch (direzione) {
    case 0:
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
