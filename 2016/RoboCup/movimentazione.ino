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
  avanzamento(22, 150);
}

void destra() {
  rotazione(87, 150);
}

void dietro() {
  avanzamento(-22, 150);
}

void sinistra() {
  rotazione(-87, 150);
}

//-------------------------------------------------------------------------------

void avanzamento(float distanzaVoluta, float velocita) {
  float gradiIniziali = gyroscope(0, true);
  int sensor = 0;
  if (distanza(0, true) > distanza(2, true))
    sensor = 2;
  float distanzaIniziale = distanza(sensor, true);
  float Kg = 10;
  float erroreGyro = 0.0;
  float erroreDist = 0.0;
  float distance = 0.0;
  float offset = 10;

  if (distanzaVoluta < 0)
    velocita = -velocita;

  while (true) {
    distance = distanza(sensor, true);
    //Serial.println(distance);
    erroreGyro = gradiIniziali - gyroscope(0, true);

    motori(velocita + erroreGyro * Kg, velocita - erroreGyro * Kg - offset);

    if (distance < 1) {
    }
    else {
      if (distance <= 7 && sensor == 0) {
        if (distanza(sensor, true) <= 7)
          break;
      }
      else if (distance <= 13  && sensor == 2) {
        if (distanza(sensor, true) <= 13)
          break;
      }
      if (abs(distanzaIniziale - distance) >= abs(distanzaVoluta))
        break;
    }

    if (-5 <= gyroscope(1, true) <= 5) {
      if (colore(1)) {
        motori(0, 0);
        aspetta(500);
        if (colore(1)) {
          aggiungiProprieta(actualX, actualY, actualL, pCellaNera);
          velocita = -velocita;
          distanzaVoluta = abs(distanzaIniziale - distanza(sensor, true));
          distanzaIniziale = distanza(sensor, true);
          gradiIniziali = gyroscope(0, true);
          while (true) {
            erroreGyro = gradiIniziali - gyroscope(0, true);
            motori(velocita + erroreGyro * Kg, velocita - erroreGyro * Kg);
            if (abs(distanzaIniziale - distanza(sensor, true)) >= distanzaVoluta)
              break;
          }
          actualX = previousX;
          actualY = previousY;
          break;
        }
      }
      /*else { //else per ostacolo !!!DEBUG!!!
        unsigned int tempoIniziale = millis();
        unsigned int currentMillis = 0;
        while (currentMillis - tempoIniziale < 1200) {
          motori(200, 200);
          currentMillis = millis();
        }
        }*/
    }
    else if (gyroscope(1, true) >= 15.0) {
      unsigned int tempoIniziale = millis();
      while (true) {
        erroreGyro = gradiIniziali - gyroscope(0, true);
        motori(200 + erroreGyro * Kg, 200 - erroreGyro * Kg);
        if (gyroscope(1, true) <= 6.0)
          break;
        if (millis() - tempoIniziale >= tempoRampa) {
          if (actualL == 0) {
            previousL = actualL;
            actualL = 1;
          }
          break;
        }
      }
    }
    else if (gyroscope(1, true) <= -15.0) {
      unsigned int tempoIniziale = millis();
      while (true) {
        erroreGyro = gradiIniziali - gyroscope(0, true);
        motori(125 + erroreGyro * Kg, 125 - erroreGyro * Kg);
        if (gyroscope(1, true) > -6.0)
          break;
        if (millis() - tempoIniziale >= tempoRampa) {
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
  float Kp = 1.0;
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
  avanzamento(3, 100);
}
