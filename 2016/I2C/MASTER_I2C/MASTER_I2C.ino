//MASTER
#include <Wire.h>

byte x = 0;
String num = "";

void setup()
{
  //inizializzo la libreria Wire come Master
  Wire.begin();
  //init seiale
  Serial.begin(9600);
  //avviso che il programma è avviato
  Serial.println("START");

}

void loop()
{
  //invio sul bus I2C un byte al device
  //che ha come indirizzo il valore 0x04
  //start trasmissione
  Wire.beginTransmission(0x04);
  //invio un byte
  Wire.write(x);
  //fine trasmissione
  Wire.endTransmission();

  delayMicroseconds(500);

  //richiedo un byte allo slave che ha indirizzo 0x04
  Wire.requestFrom(0x04, 4);

  num = "";
  while (Wire.available())
    num += (char)Wire.read();
  Serial.println(num);
}
