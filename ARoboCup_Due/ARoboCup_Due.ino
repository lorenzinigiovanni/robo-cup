

void setup() {
  
}

void loop() {
  
}

void avanzamento(float distanzaVoluta, float velocita) {
  float gradiIniziali = gyroscope(0, 0.0);
  float correzione = 0;
  float distanzaIniziale = distanza();
  float errore = 0;

  Serial.print("start deg: ") && Serial.println(gradiIniziali);

  if (gradiIniziali < 90){
    gradiIniziali += 90;
    correzione = 90;
  }
  else if (gradiIniziali > 270){
    gradiIniziali -= 90;
    correzione = 90;
  }

  Serial.print("yaw: ") && Serial.println(gradiIniziali);

  while (distanzaIniziale - distanza() <= distanzaVoluta) {
    errore = gradiIniziali - gyroscope(0, correzione);
    Serial.print("errore: ") && Serial.print(errore) && Serial.print("\t");
    Serial.print("delta D: ") && Serial.println(distanzaIniziale - distanza());
    speedMotor(cingoloA, velocita - errore);
    speedMotor(cingoloB, velocita + errore);
  }

  speedMotor(cingoloA, 0);
  speedMotor(cingoloB, 0);
}

void rotazione(float gradiVoluti, float velocita) {
  float gradiIniziali = gyroscope(0, 0.0);

  if (gradiIniziali > 180) {
    float gradiFinali = gradiIniziali - gradiVoluti;
    while (gradiFinali <= gyroscope(0, 0.0)) {
      float errore = gradiFinali - gyroscope(0, 0.0);
      speedMotor(cingoloA, -velocita);
      speedMotor(cingoloB, velocita);
    }
  }

  else {
    float gradiFinali = gradiIniziali + gradiVoluti;
    while (gradiFinali >= gyroscope(0, 0.0)) {
      speedMotor(cingoloA, velocita);
      speedMotor(cingoloB, -velocita);
    }
  }

  speedMotor(cingoloA, 0);
  speedMotor(cingoloB, 0);
}
