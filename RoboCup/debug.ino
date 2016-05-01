void seriale() {
  /*avanti();
    delay(1000);
    dietro();
    delay(1000);*/
  Serial.println(gyroscope(1, true));
  /*Serial.print("d0: ");
    Serial.println(distanza(0, false));*/
  /*Serial.print(" d1: ");
    Serial.println(distanza(1, true));*/
  /*Serial.print(" d2: ");
    Serial.println(distanza(2, true));*/
  /*Serial.print(" d3: ");
    Serial.println(distanza(3, true));*/
  /*Serial.print("t3: ");
    Serial.println(temperatura(3, true));*/
    colore(0);
  /*if (colore(1))
    Serial.println("NERO");*/
  /*servoTorrettaFunction(posSXSM1);
    servoTorrettaFunction(posAVSM1);
    servoTorrettaFunction(posDXSM1);*/
  /*avanti();
    delay(1000);
    dietro();
    delay(1000);*/
  yield();
}

//-------------------------------------------------------------------------------

void aspetta(unsigned int tempoPausa) {
  unsigned int tempoIniziale = millis();
  while (millis() - tempoIniziale <= tempoPausa) {}
}

//-------------------------------------------------------------------------------

void go_on() {
  while (digitalRead(button)) {}
  delay(300);
  Serial.println("go_on() Completed!");
}
