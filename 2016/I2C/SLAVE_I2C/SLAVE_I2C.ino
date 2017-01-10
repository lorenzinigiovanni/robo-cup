//SLAVE
#include <Wire.h>

String x = "ciao";

void setup()
{
//inizializzo la libreria
//imposto l’indirizzo dello slave
Wire.begin(0x04);

//eventi per la ricezione del dato
//e per la richiesta del dato
Wire.onReceive(receiveEvent);
Wire.onRequest(requestEvent);
}

void loop()
{
//esegui qualcosa
delay(1000);
}

void receiveEvent(int data)
{
//questo evento viene generato quando sul bus
//è presente un dato da leggere

//eseguo la lettura
/*x = Wire.read();

//elaboro il dato letto
x++;*/
}

void requestEvent()
{
//questo evento viene generato quando il master
//richiede ad uno specifico slave
//una richiesta di dati

//spedisco il dato al Master
Wire.write(x.c_str());

}
