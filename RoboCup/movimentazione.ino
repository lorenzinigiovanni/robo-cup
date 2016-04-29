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

  passaggi[actualX][actualY][actualL] += 1;

  direzione -= gyroscope(0, false);

  switch (direzione) {
    case -3:
      destra();
      avanti();
      break;
    case -2:
      dietro();
      break;
    case -1:
      sinistra();
      avanti();
      break;
    case 0:
      avanti();
      break;
    case 1:
      destra();
      avanti();
      break;
    case 2:
      dietro();
      break;
    case 3:
      sinistra();
      avanti();
      break;
  }
}

//-------------------------------------------------------------------------------

void ruota(int direzione) {
  direzione -= gyroscope(0, false);

  switch (direzione) {
    case -3:
      destra();
      break;
    case -2:
      destra();
      destra();
      break;
    case -1:
      sinistra();
      break;
    case 0:
      break;
    case 1:
      destra();
      break;
    case 2:
      sinistra();
      sinistra();
      break;
    case 3:
      sinistra();
      break;
  }
}

//-------------------------------------------------------------------------------

void avanti() {
  avanzamento(24.5, 150);
}

void destra() {
  rotazione(87, 150);
}

void dietro() {
  avanzamento(-24.5, 150);
}

void sinistra() {
  rotazione(-87, 150);
}

//-------------------------------------------------------------------------------

void avanzamento(float distanzaVoluta, float velocita) {
  float gradiIniziali = gyroscope(0, true);
  float distanzaIniziale = distanza(0, true);
  float Kg = 10;
  float Kd = 1.5;
  float erroreGyro = 0.0;
  float erroreDist = 0.0;
  float distance = 0.0;
  float offset = 10;

  if (distanzaVoluta < 0) {
    velocita = -velocita;
    Kd = -Kd;
  }

  while (true) {
    distance = distanza(0, true);
    erroreGyro = gradiIniziali - gyroscope(0, true);
    erroreDist = abs(distanzaVoluta - distanzaIniziale + distance);

    motori((velocita + erroreGyro * Kg + erroreDist * Kd) / 1.2, (velocita - erroreGyro * Kg + erroreDist * Kd) / 1.2 - offset);

    if (distance <= 7)
      if (distanza(0, true) <= 7)
        break;

    if (abs(distanzaIniziale - distance) >= abs(distanzaVoluta))
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
          erroreGyro = gradiIniziali - gyroscope(0, true);
          motori(velocita + erroreGyro * Kg, velocita - erroreGyro * Kg);
          if (abs(distanzaIniziale - distanza(0, true)) >= distanzaVoluta)
            break;
        }
        actualX = previousX;
        actualY = previousY;
        break;
      }
    }

    if (gyroscope(1, true) > 10) {
      unsigned int tempoIniziale = millis();
      while (true) {
        erroreGyro = gradiIniziali - gyroscope(0, true);
        motori(200 + erroreGyro * Kg, 200 - erroreGyro * Kg);
        if (distanza(0, true) < 10 || gyroscope(1, true) < 8) {
          if (millis() - tempoIniziale >= tempoRampa)
            if (actualL == 0) {
              previousL = actualL;
              actualL = 1;
            }
          break;
        }
      }
    }
    else if (gyroscope(1, true) < -10) {
      unsigned int tempoIniziale = millis();
      while (true) {
        erroreGyro = gradiIniziali - gyroscope(0, true);
        motori(125 + erroreGyro * Kg, 125 - erroreGyro * Kg);
        if (distanza(0, true) < 10 || gyroscope(1, true) > -8) {
          if (millis() - tempoIniziale >= tempoRampa)
            if (actualL == 1) {
              previousL = actualL;
              actualL = 0;
            }
          break;
        }
      }
    }
  }

  motori (0, 0);
}

//-------------------------------------------------------------------------------

void rotazione(float gradiVoluti, float velocita) {
  float gradiIniziali = gyroscope(0, true);
  float Kp = 1.0;//0.3;
  float errore = 0;
  float gradiAttuali = gradiIniziali;

  if (gradiVoluti < 0) {
    velocita = -velocita;
    Kp = -Kp;
  }

  while (true) {
    gradiAttuali = gyroscope(0, true);
    errore = abs(gradiVoluti + gradiIniziali - gradiAttuali);
    motori(velocita + errore * Kp, -velocita - errore * Kp);
    if (abs(gradiIniziali - gradiAttuali) >= abs(gradiVoluti))
      break;
  }

  motori (0, 0);
}

//-------------------------------------------------------------------------------

void azzeraCulo() {
  motori(-255, -255);
  delay(1000);
  motori(0, 0);
  delay(500);
  avanzamento(5, 100);
}
