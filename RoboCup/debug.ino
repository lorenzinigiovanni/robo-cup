void seriale() {
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
  yield();
}

//-------------------------------------------------------------------------------

void display() {
  int g0t = gyroscope(0, true);
  int g2t = gyroscope(2, true);
  int g0f = gyroscope(0, false);
  int g2f = gyroscope(2, false);

  lcd.clear();
  lcd.print(g0t);
  lcd.print(" ");
  lcd.print(g2t);
  lcd.setCursor(0, 1);
  lcd.print(g0f);
  lcd.print(" ");
  lcd.print(g2f);
  //yield();
}
