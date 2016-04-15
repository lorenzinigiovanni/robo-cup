void seriale() {
<<<<<<< HEAD
=======
  Serial.print("gyroscopio 0: ");
  Serial.print(gyroscope(0, true));
  Serial.print(" temp SX: ");
  Serial.print(temperatura(-1, true));
  Serial.print(" temp AV: ");
  Serial.print(temperatura(0, true));
  Serial.print(" temp DX: ");
  Serial.print(temperatura(1, true));
  Serial.print(" temp DI: ");
  Serial.print(temperatura(2, true));
  Serial.print(" dist SX: ");
  Serial.print(distanza(-1, true));
  Serial.print(" dist AV: ");
  Serial.print(distanza(0, true));
  Serial.print(" dist DX: ");
  Serial.print(distanza(1, true));
  Serial.print(" dist DI: ");
  Serial.print(distanza(2, true));
  //Serial.print(" R: ");
  //Serial.print(" G: ");
  //Serial.print(" B: ");
  Serial.write(10);
>>>>>>> fb7d1296f6f78e3d74fb1768bbb5a8f1b37602ea
  yield();
}

//-------------------------------------------------------------------------------

void display() {
}

//-------------------------------------------------------------------------------

void aspetta(int tempoPausa) {
  int tempoIniziale = millis();
  while (millis() - tempoIniziale < tempoPausa) {}
}
