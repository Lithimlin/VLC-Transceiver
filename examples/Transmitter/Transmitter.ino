#include <Timer.h> // Die Timer Bibilothek mit einbinden (Timer 2.1)

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
// Einen Timer initialisieren
Timer t;
// Einen Boolean anlegen, um zwischen dem Bild und dem String zu wechseln
bool sendImage = false;

void setup() {
  // Die serielle Kommunikation mit dem seriellen Monitor starten
  Serial.begin(2000000);

  // Pins initialisieren
  pinMode(RED, INPUT);
  pinMode(BLUE, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  // Blaue LED einschalten
  digitalWrite(LED_BLUE, HIGH);

  // Ein Event an den Timer binden, das alle 5 Sekunden "sendData" ausführt
  int8_t sendEvent = t.every(5 * 1000, sendData, (void*)0);

  // Den Transmitter starten
  transceiver.startTransmitter();
}

void loop() {
  // Den Timer aktualisieren
  t.update();
  if(digitalRead(RED)) { // Wenn der rote Button gedrückt wird...
    sendImage = true;  //... soll als nächstes das Bild geschickt werden
    // Lasse die LEDs entsprechend leuchten.
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_RED, HIGH);
  }
  if(digitalRead(BLUE)) { // Wenn der blaue Button gedrückt wird...
    sendImage = false; //... soll als nächstes der String gesendet werden.
    // Lasse die LEDs entsprechend leuchten.
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_RED, LOW);
  }
}

void sendData(void){
  if(sendImage) { // Wenn gerade das Bild gesendet werden soll...
    //... sende das vorher initialisierte Bild...
    transceiver.sendData(image);
    Serial.println("Bild gesendet");
  } else { // Ansonsten...
    //... sende den vorher initialisierten String
    transceiver.sendData(string);
    Serial.println("String gesendet");
  }
}
