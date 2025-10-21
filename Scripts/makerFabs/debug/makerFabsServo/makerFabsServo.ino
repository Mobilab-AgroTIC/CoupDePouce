#include <ESP32Servo.h>

static const int SERVO_PIN = 38;     // GPIO38
Servo servo;

void setup() {
  // Min/Max impulsions selon ton servo (500–2500 µs marche pour la plupart)
  servo.attach(SERVO_PIN, 500, 2500);
  // Position neutre
  servo.write(90);
}

void loop() {
  // Petit sweep pour test
  for (int a = 30; a <= 150; a++) { servo.write(a); delay(15); }
  for (int a = 150; a >= 30; a--) { servo.write(a); delay(15); }
}
