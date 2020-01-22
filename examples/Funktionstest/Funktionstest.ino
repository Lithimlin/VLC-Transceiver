#include <Max72xxPanel.h>

#include <Constants.h>
#include <LEDBitmap.h>

// Das Display auf dem Matrix Pin initialisieren
Max72xxPanel matrix = Max72xxPanel(MATRIX_PIN);

void setup() {
  // Stelle die Buttons als Eingang ein
  pinMode(YELLOW, INPUT);
  pinMode(RED, INPUT);
  pinMode(BLUE, INPUT);
  pinMode(GREEN, INPUT);

  // Stelle die LEDs als Ausgang ein
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // Serielle Kommunikation mit dem Display starten
  Serial.begin(9600);

  // Das Display konfigurieren
  matrix.setRotation(0, 2);
  // Das Display leeren
  matrix.fillScreen(LOW);
  matrix.write();

  // Die LEDs kurz blinken lassen
  digitalWrite(LED_BLUE, HIGH);
  Serial.println("LED Blue");
  delay(500); // mit einer halben Sekunde Verzögerung
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_RED, HIGH);
  Serial.println("LED Red");
  delay(500);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  Serial.println("LED Green");
  delay(500);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  Serial.println("LED Yellow");
  delay(500);
  digitalWrite(LED_YELLOW, LOW);

  matrix.drawChar(1, 0, '0', HIGH, LOW, 1);
  matrix.write();
}

void loop() {
  // Gebe auf dem Display ein "Y" aus, wenn gelb gedrückt wird
  if(digitalRead(YELLOW)) {
    matrix.fillScreen(LOW);
    matrix.drawChar(1, 0, 'Y', HIGH, LOW, 1);
    matrix.write();
    Serial.println("Yellow");
  }
  // Gebe auf dem Display ein "R" aus, wenn rot gedrückt wird
  else if(digitalRead(RED)) {
    matrix.fillScreen(LOW);
    matrix.drawChar(1, 0, 'R', HIGH, LOW, 1);
    matrix.write();
    Serial.println("Red");
  }
  // Gebe auf dem Display ein "B" aus, wenn blau gedrückt wird
  else if(digitalRead(BLUE)) {
    matrix.fillScreen(LOW);
    matrix.drawChar(1, 0, 'B', HIGH, LOW, 1);
    matrix.write();
    Serial.println("Blue");
  }
  // Gebe auf dem Display ein "G" aus, wenn grün gedrückt wird
  else if(digitalRead(GREEN)) {
    matrix.fillScreen(LOW);
    matrix.drawChar(1, 0, 'G', HIGH, LOW, 1);
    matrix.write();
    Serial.println("Green");
  }
}
