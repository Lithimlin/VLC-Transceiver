#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

#include <Constants.h>
#include <Transciever.h>
#include <LEDBitmap.h>

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

// Das Display auf dem Matrix Pin mit einer Höhe und Breite von je einem Display initialisieren
Max72xxPanel matrix = Max72xxPanel(MATRIX_PIN, 1, 1);
// Den Transmitter initialisieren
Transciever transciever;
// Eine Variable für die letzte Empfangszeit initialisieren
long lastReception;

void setup() {
  // Serielle Kommunikation mit dem Display starten
  Serial.begin(9600);
  // Das Display konfigurieren
  matrix.setIntensity(5);
  matrix.setRotation(0, 3);
  // Das Display leeren
  matrix.fillScreen(LOW);
  matrix.write();
  // Den Reciever starten
  transciever.startReciever();
  transciever.startTransmitter();

  lastReception = millis(); //Den Startwert für die letzte Empfangszeit auf den Einschaltzeitpunkt legen
}

void loop() {
  // Wenn ein Bild erfolgreich empfangen wurde...
  if(transciever.receptionSuccessful()) {
    lastReception = millis(); //... speichere die Zeit,...
    matrix.fillScreen(LOW); //... leere das Display...
    matrix.drawBitmap(0, 0, transciever.getImage().getBitmap(), 8, 8, HIGH, LOW); //... und zeige das empfangene Bild an.
    matrix.write();
  }
  if((millis() - lastReception) >= 5000) {// Wenn seit 5 Sekunden oder mehr kein Bild mehr empfangen wurde...
    matrix.fillScreen(LOW); //... leere das Display
    matrix.write();
  }
  if(digitalRead(BUTTON_ENTER) == HIGH) {
    transciever.sendData(image);
  }
}
