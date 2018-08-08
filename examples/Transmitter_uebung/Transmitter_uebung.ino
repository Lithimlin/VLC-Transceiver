#include <Timer.h> // Die Timer Bibilothek mit einbinden

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
// Einen Timer initialisieren
Timer t;

void setup() {
  // Die serielle Kommunikation mit dem Display starten
  // !!! TODO !!! //
  // Ein Event an den Timer binden, das alle 5 Sekunden "sendData" ausführt
  int8_t sendEvent = t.every(5 * 1000, sendData, (void*)0);
  // Den Transmitter starten
  // !!! TODO !!! //
}

void loop() {
  // Den Timer aktualisieren
  // !!! TODO !!! //
}
	
void sendData(void){
  // Das vorher initialisierte Bild senden 
  // !!! TODO !!! //
}
