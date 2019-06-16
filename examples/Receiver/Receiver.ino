#include <Max72xxPanel.h>

#include <Constants.h>
#include <Transceiver.h>
#include <LEDBitmap.h>

// Das Display auf dem Matrix Pin initialisieren
Max72xxPanel matrix = Max72xxPanel(MATRIX_PIN);
// Den Transmitter initialisieren
Transceiver transceiver;
// Eine Variable für die letzte Empfangszeit initialisieren
long lastReception;

void setup() {
  // Serielle Kommunikation mit dem Display starten
  Serial.begin(2000000);
  Serial.println("Serial initialized");
  // Das Display konfigurieren
  matrix.setIntensity(8);
  matrix.setRotation(0, 2);
  // Das Display leeren
  matrix.fillScreen(LOW);
  matrix.write();

  // Den Receiver starten
  transceiver.startReceiver();

  lastReception = millis(); //Den Startwert für die letzte Empfangszeit auf den Einschaltzeitpunkt legen
  Serial.println("Ready");
}

void loop() {
  // Wenn ein Bild erfolgreich empfangen wurde...
  if(transceiver.receptionSuccessful()) {
    lastReception = millis(); //... speichere die Zeit,...
    matrix.fillScreen(LOW); //... und leere das Display.

    switch(transceiver.getType()) { // Schaue nach dem Datentyp der empfangenen Übertragung:
      case 1: { // Falls es ein String ist...
          Serial.println();
          Serial.println(transceiver.getString()); //... gebe ihn auf dem seriellen Monitor aus.
          Serial.println();
          matrix.scrollDrawText(transceiver.getString());
        } break;

      case 2: { // Falls es ein Bild ist...
          matrix.drawBitmap(0, 0, transceiver.getImage().getBitmap(), 8, 8, HIGH, LOW); //... zeige das empfangene Bild an.
          matrix.write();
        } break;
    }
  }
  if((millis() - lastReception) >= 5000) {// Wenn seit 5 Sekunden oder mehr kein String oder Bild mehr empfangen wurde...
    matrix.fillScreen(LOW); //... leere das Display
    matrix.write();
  }
}
