void muovi(int direzione) {
  switch (direzione) {
    case 0:
      actualX++;
      break;
    case 1:
      actualY++;
      break;
    case 2:
      actualX--;
      break;
    case 3:
      actualY--;
      break;
  }

  direzione -= gyroscope(0, false);

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
  float distanzaIniziale = distanza(0, true);
  float Kp = 5;
  float errore = 0;

  if (distanzaVoluta < 0)
    velocita = -velocita;

  distanzaVoluta = abs(distanzaVoluta);

  while (true) {
    errore = gradiIniziali - gyroscope(0, true);
    motori(velocita + errore * Kp, velocita - errore * Kp);
    if (abs(distanzaIniziale - distanza(0, true)) >= distanzaVoluta)
      break;
    if (colore(1)) {
      motori(0, 0);
      aspetta(500);
      if (colore(1)) {
        aggiungiProprieta(actualX, actualY, actualL, pCellaNera);
        velocita = -velocita;
        distanzaVoluta = distanzaIniziale - distanza(0, true);
        distanzaIniziale = distanza(0, true);
        gradiIniziali = gyroscope(0, true);
        while (true) {
          errore = gradiIniziali - gyroscope(0, true);
          motori(velocita + errore * Kp, velocita - errore * Kp);
          if (abs(distanzaIniziale - distanza(0, true)) >= distanzaVoluta)
            break;
        }
        actualX = previousX;
        actualY = previousY;
        break;
      }
    }
    if (gyroscope(2, true) > 10) {
      while (true) {
        errore = gradiIniziali - gyroscope(0, true);
        motori(150 + errore * Kp, 150 - errore * Kp);
        if (distanza(0, true) < 10 || gyroscope(2, true) < 8)
          break;
      }
    }
    else if (gyroscope(2, true) < -10) {
      while (true) {
        errore = gradiIniziali - gyroscope(0, true);
        motori(100 + errore * Kp, 100 - errore * Kp);
        if (distanza(0, true) < 10 || gyroscope(2, true) > -8)
          break;
      }
    }
  }

  motori (0, 0);
}

//-------------------------------------------------------------------------------

void rotazione(float gradiVoluti, float velocita) {
  float gradiIniziali = gyroscope(0, true);
  float Kp = 0.2;
  float errore = 0;
  float gradiAttuali = gradiIniziali;

  if (gradiVoluti < 0) {
    velocita = -velocita;
    Kp = -Kp;
  }

  gradiVoluti = abs(gradiVoluti);

  while (true) {
    gradiAttuali = gyroscope(0, true);
    errore = gradiVoluti + gradiIniziali - gradiAttuali;
    motori(velocita + errore * Kp, -velocita - errore * Kp);
    if (abs(gradiIniziali - gradiAttuali) >= gradiVoluti)
      break;
  }

  motori (0, 0);
}
