#include <Matrix.h>

#include <Constants.h>
#include <Transceiver.h>
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
// Einen String anlegen
String string("Hello VLC!");

// Das Display auf dem Matrix Pin initialisieren
Matrix matrix = Matrix(MATRIX_PIN);
// Den Transmitter initialisieren
Transceiver transceiver;
// Eine Variable für die letzte Empfangszeit initialisieren
long lastReception;

void setup() {
  // Serielle Kommunikation mit dem Display starten
  Serial.begin(9600);

  // Pins initialisieren
  pinMode(RED, INPUT);
  pinMode(BLUE, INPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_RED, LOW);

  // Das Display konfigurieren
  matrix.setRotation(0, 2);
  // Das Display leeren
  matrix.fillScreen(LOW);
  matrix.write();
  // Den Receiver und Transmitter starten
  transceiver.startReceiver();
  transceiver.startTransmitter();

  lastReception = millis(); //Den Startwert für die letzte Empfangszeit auf den Einschaltzeitpunkt legen
}

void loop() {
  // Wenn eine Nachricht erfolgreich empfangen wurde...
  if(transceiver.handleReception(&matrix)){ //... verarbeite sie mit der Matrix und...
    lastReception = millis(); //... speichere die Zeit.
  }
  
  /*if(transceiver.receptionSuccessful()) {
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
          matrix.drawImage(0, 0, transceiver.getImage()); //... zeige das empfangene Bild an.
          matrix.write();
        } break;
    }
  }*/
  
  if((millis() - lastReception) >= 5000) {// Wenn seit 5 Sekunden oder mehr kein Bild mehr empfangen wurde...
    matrix.fillScreen(LOW); //... leere das Display
    matrix.write();
  }
  if(digitalRead(RED)) {
    transceiver.sendData(image);
    digitalWrite(LED_RED, HIGH);
    delay(200);
    digitalWrite(LED_RED, LOW);
  }
  if(digitalRead(BLUE)) {
    transceiver.sendData(string);
    digitalWrite(LED_BLUE, HIGH);
    delay(200);
    digitalWrite(LED_BLUE, LOW);
  }
}
