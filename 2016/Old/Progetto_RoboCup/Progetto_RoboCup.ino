#include "MeOrion.h"
#include <SoftwareSerial.h>

String readBuf;
float ypr[3] = {0.0, 0.0, 0.0};

MeDCMotor cingoloA(M1);
MeDCMotor cingoloB(M2);
MeRGBLed led(PORT_6);
MeUltrasonicSensor ultraSensor(PORT_7);

void setup() {
  Serial.begin(115200);
  
  
  delay (10000);
  avanzamento (30, 50);
  delay (10000);
  rotazione (90, 30);
  
}

void loop() {
  //Serial.println(gyroscope(0, 0.0));
//  delay (100);
}

void speedMotor(MeDCMotor motore, int speed) {
  motore.run(speed);
}

float distanza() {
  return ultraSensor.distanceCm();
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

float ritorno = 0.0;

float gyroscope(int sel, float correzione) {
  if (Serial.available()) {
    readBuf = Serial.readStringUntil('\n');
    int posComma = readBuf.indexOf(',');
    int posSemicolon = readBuf.indexOf(';');

    ypr[0] = readBuf.substring(0, posComma - 1).toFloat();
    ypr[1] = readBuf.substring(posComma + 1, posSemicolon - 1).toFloat();
    ypr[2] = readBuf.substring(posSemicolon + 1).toFloat();
  }
  
  ritorno = ypr[sel];

  if (sel == 0) {
    ritorno += correzione;
    if (ritorno > 360.0)
      ritorno -= 360.0;
  }
  
  return ritorno;
}
