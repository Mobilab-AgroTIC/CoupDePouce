#include <Arduino.h>

// ---- Pins pour LilyGO T-A7670 (selon RNT) ----
#define MODEM_BAUDRATE      115200
#define MODEM_TX_PIN        26
#define MODEM_RX_PIN        27
#define BOARD_POWERON_PIN   12
#define BOARD_PWRKEY_PIN    4
#define MODEM_RESET_PIN     5

HardwareSerial SerialAT(1);
String currentCommand = "";  // pour afficher ce qu'on envoie

void powerOnModem() {
  pinMode(BOARD_POWERON_PIN, OUTPUT);
  digitalWrite(BOARD_POWERON_PIN, HIGH);
  delay(50);

  pinMode(MODEM_RESET_PIN, OUTPUT);
  digitalWrite(MODEM_RESET_PIN, HIGH);

  pinMode(BOARD_PWRKEY_PIN, OUTPUT);
  digitalWrite(BOARD_PWRKEY_PIN, HIGH);
  delay(100);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);
  delay(1600);
  digitalWrite(BOARD_PWRKEY_PIN, HIGH);
  delay(3000);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n[T-A7670] Bridge AT (commandes visibles)\n");

  powerOnModem();
  SerialAT.begin(MODEM_BAUDRATE, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
  delay(1000);

  Serial.println("Tape tes commandes AT ci-dessous (ex: AT, AT+CPIN?, ...)");
  Serial.println("------------------------------------------------------");
}

void loop() {
  // --- Lecture entrée PC ---
  while (Serial.available()) {
    char c = Serial.read();
    SerialAT.write(c);
    currentCommand += c;

    // quand l'utilisateur appuie sur Entrée (\r ou \n)
    if (c == '\r' || c == '\n') {
      if (currentCommand.length() > 1) {
        Serial.print(F("\n>> "));     // préfixe pour la commande envoyée
        Serial.println(currentCommand);
      }
      currentCommand = "";
    }
  }

  // --- Lecture retour modem ---
  while (SerialAT.available()) {
    String r = SerialAT.readStringUntil('\n');
    r.trim();
    if (r.length()) {
      Serial.print(F("<< "));  // préfixe pour la réponse du modem
      Serial.println(r);
    }
  }
}
