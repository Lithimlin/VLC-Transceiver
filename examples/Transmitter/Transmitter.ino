#include <Constants.h>
#include <LEDBitmap.h>
#include <Transceiver.h>

// Den Transceiver initialisieren
Transceiver transceiver;

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

void setup() {
  // Die serielle Kommunikation mit dem seriellen Monitor starten
  Serial.begin(9600);

  // Pins initialisieren
  pinMode(RED, INPUT);
  pinMode(BLUE, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  // Blaue LED einschalten
  digitalWrite(LED_BLUE, HIGH);

  // Den Transmitter starten
  transceiver.startTransmitter();
}

void loop() {
  // Den Timer aktualisieren
  if(digitalRead(RED)) { // Wenn der rote Button gedrückt wird...
    //... sende das vorher initialisierte Bild...
    transceiver.sendData(image);
    Serial.println("Bild gesendet");
    digitalWrite(LED_RED, HIGH); //... und schalte die rote LED kurz an...
    delay(500);
    digitalWrite(LED_RED, LOW); //... und wieder aus.
  }
  
  if(digitalRead(BLUE)) { // Wenn der blaue Button gedrückt wird...
    //... sende den vorher initialisierten String...
    transceiver.sendData(string);
    Serial.println("String gesendet");
    digitalWrite(LED_BLUE, HIGH); //... und schalte die blaue LED kurz an...
    delay(500);
    digitalWrite(LED_BLUE, LOW); //... und wieder aus.
  }
}
