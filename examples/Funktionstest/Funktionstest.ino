#include <Timer.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

#include <Constants.h>
#include <LEDBitmap.h>

// Das Display auf dem Matrix Pin mit einer Höhe und Breite von je einem Display initialisieren
Max72xxPanel matrix = Max72xxPanel(MATRIX_PIN, 1, 1);

void setup() {
  // Stelle die Buttons als Eingang ein
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);
  pinMode(BUTTON_ENTER, INPUT);
  // Stelle die LEDs als Ausgang ein
  pinMode(LED_STATUS_1, OUTPUT);
  pinMode(LED_STATUS_2, OUTPUT);
  // Serielle Kommunikation mit dem Display starten
  Serial.begin(2000000);
  // Das Display konfigurieren
  matrix.setIntensity(8);
  matrix.setRotation(0, 3);
  // Das Display leeren
  matrix.fillScreen(LOW);
  matrix.write();
  // Die LEDs kurz blinken lassen
  digitalWrite(BLUE, HIGH);
  delay(500); // mit einer halben Sekunde Verzögerung
  digitalWrite(BLUE, LOW);
  digitalWrite(RED, HIGH);
  delay(500);
  digitalWrite(RED, LOW);
}

void loop() {
  // Gebe auf dem Display ein "D" aus, wenn "RUNTER" gedrückt wird
  if(digitalRead(BUTTON_DOWN) == HIGH) {
    matrix.fillScreen(LOW);
    matrix.drawChar(1, 0, 'D', HIGH, LOW, 1);
    matrix.write();
  }
  // Gebe auf dem Display ein "U" aus, wenn "HOCH" gedrückt wird
  else if(digitalRead(BUTTON_UP) == HIGH) {
    matrix.fillScreen(LOW);
    matrix.drawChar(1, 0, 'U', HIGH, LOW, 1);
    matrix.write();
  }
  // Gebe auf dem Display ein "L" aus, wenn "LINKS" gedrückt wird
  else if(digitalRead(BUTTON_LEFT) == HIGH) {
    matrix.fillScreen(LOW);
    matrix.drawChar(1, 0, 'L', HIGH, LOW, 1);
    matrix.write();
  }
  // Gebe auf dem Display ein "R" aus, wenn "RECHTS" gedrückt wird
  else if(digitalRead(BUTTON_RIGHT) == HIGH) {
    matrix.fillScreen(LOW);
    matrix.drawChar(1, 0, 'R', HIGH, LOW, 1);
    matrix.write();
  }
  // Gebe auf dem Display ein "E" aus, wenn "ENTER" gedrückt wird
  else if(digitalRead(BUTTON_ENTER) == HIGH) {
    matrix.fillScreen(LOW);
    matrix.drawChar(1, 0, 'E', HIGH, LOW, 1);
    matrix.write();
  }
}
