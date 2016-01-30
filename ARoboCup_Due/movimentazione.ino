void muovi(int direzione) {
  switch (direzione) {
    case 0:
      actualX ++;
      break;
    case 1:
      actualY ++;
      break;
    case 2:
      actualX --;
      break;
    case 3:
      actualY --;
      break;
  }
  direzione = direzione - gyroscope(0, false);

  switch (direzione) {
    case -3:
      destra();
      break;
    case -2:
      dietro();
      break;
    case -1:
      sinistra();
      break;
    case 0:
      avanti();
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

//-------------------------------------------------------------------------------

void avanti() {
  avanzamento(30, 100);
}

void destra() {
  rotazione(90, 100);
  avanzamento(30, 100);
}

void dietro() {
  avanzamento(-30, 100);
}

void sinistra() {
  rotazione(-90, 100);
  avanzamento(30, 100);
}

//-------------------------------------------------------------------------------

void avanzamento(float distanzaVoluta, float velocita) {
  float gradiIniziali = gyroscope(0, true);
  //float distanzaIniziale = distanza(0, true);
  float Kp = 5;

  if (distanzaVoluta > 0) {
    int i = 0; //da cancellare
    //while (distanzaIniziale - distanza(0, true) < distanzaVoluta) {
    while (i < 100) { //da cancellare
      float errore = gradiIniziali - gyroscope(0, true);
      motori(velocita + errore * Kp, velocita - errore * Kp);
      i++;
    }
  }

  else if (distanzaVoluta < 0) {
    int i = 0; //da cancellare
    //while (distanzaIniziale - distanza(0, true) < distanzaVoluta) {
    while (i < 100) { //da cancellare
      float errore = gradiIniziali - gyroscope(0, true);
      motori(-velocita + errore * Kp, -velocita - errore * Kp);
      i++;
    }
  }

  motori (0, 0);
}

//-------------------------------------------------------------------------------

void rotazione(float gradiVoluti, float velocita) {
  float gradiIniziali = gyroscope(0, true);
  float gradiFinali = gradiIniziali + gradiVoluti;
  float K = 0.2;

  if (gradiVoluti > 0) {
    while (gradiFinali > gyroscope(0, true)) {
      float errore = gradiFinali - gyroscope(0, true);
      motori (velocita + errore * K, -velocita - errore * K);
    }
  }

  else if (gradiVoluti < 0) {
    while (gradiFinali < gyroscope(0, true)) {
      float errore = gyroscope(0, true); - gradiFinali;
      motori (-velocita - errore * K, velocita + errore * K);
    }
  }

  motori (0, 0);
}
