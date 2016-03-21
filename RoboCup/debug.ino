void seriale() {
  Serial.println(gyroscope(0, true));
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
