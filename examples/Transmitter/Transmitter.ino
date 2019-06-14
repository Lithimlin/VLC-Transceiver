#include <Timer.h> // Die Timer Bibilothek mit einbinden (Timer 2.1)

#include <Constants.h>
#include <LEDBitmap.h>
#include <Transciever.h>

// Den Transciever initialisieren
Transciever transciever;

// Ein Datenarray initialisieren
int data[] = {1, 1, 1, 0, 0, 1, 1, 1,
              1, 0, 0, 0, 1, 0, 0, 0,
              1, 1, 0, 0, 1, 0, 0, 1,
              1, 0, 0, 0, 0, 1, 1, 1,
              0, 1, 0, 0, 0, 0, 0, 0,
              0, 1, 0, 0, 1, 1, 1, 0,
              0, 1, 0, 0, 0, 1, 0, 0,
              0, 1, 1, 1, 0, 1, 0, 0};
// Mit dem Datenarray ein Bild initialisieren
LEDBitmap image(8, 8, data);
// Einen String anlegen
String string = "Hello World!";
// Einen Timer initialisieren
Timer t;
// Einen Boolean anlegen, um zwischen dem Bild und dem String zu wechseln
bool sendImage = true;

void setup() {
  // Die serielle Kommunikation mit dem Display starten
  Serial.begin(2000000);
  // Ein Event an den Timer binden, das alle 5 Sekunden "sendData" ausführt
  int8_t sendEvent = t.every(5 * 1000, sendData, (void*)0);
  // Den Transmitter starten
  transciever.startTransmitter();
}

void loop() {
  // Den Timer aktualisieren
  t.update();
}

void sendData(void){
  if(sendImage) { // Wenn gerade das Bild gesendet werden soll...
    //... sende das vorher initialisierte Bild
    transciever.sendData(image);
  } else { // Ansonsten...
    //... sende den vorher initialisierten String
    transciever.sendData(string);
  }
  // Wechsle den Status
  sendImage = !sendImage;
}
