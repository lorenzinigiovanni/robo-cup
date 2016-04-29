void program() {
  if (cellCounter > 4) {
    if (proprieta(actualX, actualY, actualL, pMuroPosizione0)) {
      ruota(2);
      azzeraCulo();
      cellCounter = 0;
    }
    else if (proprieta(actualX, actualY, actualL, pMuroPosizione1)) {
      ruota(3);
      azzeraCulo();
      cellCounter = 0;
    }
    else if (proprieta(actualX, actualY, actualL, pMuroPosizione2)) {
      ruota(0);
      azzeraCulo();
      cellCounter = 0;
    }
    else if (proprieta(actualX, actualY, actualL, pMuroPosizione3)) {
      ruota(1);
      azzeraCulo();
      cellCounter = 0;
    }
  }
  mappatura(actualX, actualY, actualL);
  percorso(actualX, actualY, actualL);
}
