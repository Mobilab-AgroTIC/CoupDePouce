#include <Arduino.h> 
#include <ESP32Servo.h>

// === CODE PIN ET NUMEROS ===

// Mets ton code PIN entre les guillemets ci-dessous, ex: "1234". Laisse "" si pas de PIN.
#define SIM_PIN "0000"     

// Remplace les numéros ci-dessous par les numéros de téléphone (Sans espace, et au format international (+33 pour la France))
// Chaque numéro est entre guillemets "" et séparés par une virgule "," (après le dernier numéro, pas de virgule).

// === UART modem (A7670E) ===
HardwareSerial SerialAT(2);
#define IO_RXD2        47
#define IO_TXD2        48
#define IO_GSM_PWRKEY   4


// --------- Utils AT ----------
static void purgeInput(uint32_t ms=50) {
  uint32_t t0 = millis();
  while (millis() - t0 < ms) {
    while (SerialAT.available()) (void)SerialAT.read();
    delay(1);
  }
}

static void sendAT(const char* cmd) {
  Serial.print(">> "); Serial.println(cmd);
  SerialAT.print(cmd); SerialAT.print("\r\n");
}

bool waitFor(const char* token, uint32_t timeout_ms = 3000) {
  uint32_t t0 = millis();
  String buf;
  while (millis() - t0 < timeout_ms) {
    while (SerialAT.available()) {
      char c = (char)SerialAT.read();
      buf += c;
      if (buf.indexOf(token) >= 0) return true;
    }
    delay(1);
  }
  return false;
}


// --------- Power modem ----------
static void modem_power_on() {
  pinMode(IO_GSM_PWRKEY, OUTPUT);
  digitalWrite(IO_GSM_PWRKEY, HIGH);
  delay(100);
  digitalWrite(IO_GSM_PWRKEY, LOW);   // A76xx: impulsion basse ~1.2s
  delay(1200);
  digitalWrite(IO_GSM_PWRKEY, HIGH);
  delay(3000);
}

// --------- Réseau ----------
static bool waitNetwork(uint32_t max_ms = 60000) {
  uint32_t t0 = millis();
  while (millis() - t0 < max_ms) {
    sendAT("AT+CEREG?");
    String buf; uint32_t t1 = millis();
    while (millis() - t1 < 1000) {
      while (SerialAT.available()) buf += (char)SerialAT.read();
    }
    Serial.print(buf);
    if (buf.indexOf(",1") >= 0 || buf.indexOf(",5") >= 0) return true; // registered / roaming
    delay(800);
  }
  return false;
}

// --------- Envoi SMS ----------
static bool sendSMS(const String& text, const char* number) {
  purgeInput();
  Serial.printf("Envoi SMS à %s : \"%s\"\n", number, text.c_str());

  SerialAT.print("AT+CMGS=\"");
  SerialAT.print(number);
  SerialAT.print("\"\r\n");

  if (!waitFor(">", 5000)) { 
    Serial.println("! Prompt '>' non reçu");
    return false; 
  }

  SerialAT.print(text);
  SerialAT.write(0x1A); // Ctrl-Z

  if (!waitFor("OK", 15000)) {
    Serial.println("! OK final non reçu");
    return false;
  }
  return true;
}
static void sendSMSAll(const String& text) {
  for (size_t i=0;i<N_DESTS;i++) {
    bool ok = sendSMS(text, DESTS[i]);
    Serial.printf("[SMS -> %s] %s\n", DESTS[i], ok ? "OK" : "ECHEC");
    delay(400);
  }
}

// --------- Servo ----------
static void moveServo(int minAngle, int maxAngle) {
  if (minAngle < 0) minAngle = 0;
  if (maxAngle > 180) maxAngle = 180;

  myservo.attach(SERVO_PIN, 500, 2500);
  for (int p = minAngle; p <= maxAngle; p++) { myservo.write(p); delay(15); }
  delay(1000);
  for (int p = maxAngle; p >= minAngle; p--) { myservo.write(p); delay(15); }
  delay(300);
  myservo.detach();
  pinMode(SERVO_PIN, INPUT_PULLDOWN); // évite que le fil signal flotte
}


// --------- SETUP ----------
void setup() {
  Serial.begin(115200);
  delay(1200);
  Serial.println("\n[CoupDePouce - ESP32-S3 + A7670E | Fire&Forget SMS]");

  SerialAT.begin(115200, SERIAL_8N1, IO_RXD2, IO_TXD2);
  SerialAT.setRxBufferSize(4096);

  modem_power_on();

  // Sync modem
  for (int i = 0; i < 5; i++) {
   sendAT("AT");
   if (waitFor("OK")) break;
   delay(100);
  }

  // Echo off, erreurs verbeuses (facultatif)
  sendAT("ATE0");
  sendAT("AT+CMEE=2");

  // PIN ?
  sendAT("AT+CPIN?");
  String r; 
  uint32_t t=millis();
  while (millis()-t < 1200) { 
    while (SerialAT.available()) r += (char)SerialAT.read(); }
  if (r.indexOf("READY") >= 0) {
      Serial.println("SIM OK");
    } else if (r.indexOf("SIM PIN") >= 0) {
      if (SIM_PIN && *SIM_PIN) {
        SerialAT.printf("AT+CPIN=\"%s\"\r\n", SIM_PIN);
        waitFor("READY", 8000);
      } else {
        Serial.println("PIN requis (SIM_PIN vide)");
      }
    } else if (r.indexOf("SIM PUK") >= 0) {
      Serial.println("SIM bloquée (PUK)");
    } else if (r.indexOf("NOT INSERTED") >= 0) {
      Serial.println("Pas de SIM");
    }


  Serial.println("Attente réseau...");
  if (!waitNetwork(60000)) Serial.println("Réseau KO (timeout)");
  else Serial.println("Réseau OK");

  // Mode SMS texte, alphabet GSM
  sendAT("AT+CMGF=1");
  sendAT("AT+CSCS=\"GSM\"");

  // CNMI=2,2 : pas de stockage, le modem “push” le SMS sur l’UART immédiatement.
  sendAT("AT+CNMI=2,2,0,0,0");

  // Vider d’éventuels anciens SMS stockés (pour éviter toute lecture différée)
  sendAT("AT+CPMS=\"SM\",\"SM\",\"SM\"");
  sendAT("AT+CMGD=1,4");
  sendAT("AT+CPMS=\"ME\",\"ME\",\"ME\"");
  sendAT("AT+CMGD=1,4");

  // Message de démarrage
  sendSMSAll("CoupDePouce operationnel. Envoyez '1' pour activer.");

  // Petit mouvement de test
  moveServo(0,20);
}

// --------- LOOP (URC only) ----------
void loop() {
  // On lit ligne par ligne. Si on voit '+CMT:' -> la prochaine ligne est le corps du SMS.
  static String line;
  static bool waitBody = false;
  static String lastHeader;

  while (SerialAT.available()) {
    char c = (char)SerialAT.read();
    if (c == '\r') continue;
    if (c != '\n') {
      line += c;
      continue;
    }

    // Fin de ligne
    String cur = line;
    line = "";

    if (cur.length() == 0) continue; // ignore lignes vides

    Serial.print("[URC] "); Serial.println(cur);

    if (waitBody) {
      // 'cur' = corps du SMS en mode texte
      String body = cur;
      body.replace("\"", "");
      body.trim();

      Serial.print("SMS reçu (body): "); Serial.println(body);

      // Seuil d'action : "1", " 1", "1 "
      if (body == "1" || body == "1 " || body == " 1") {
        Serial.println(">> ALLUMAGE");
        sendSMSAll("allumage...");
        moveServo(0, 90);
      } else {
        Serial.println("Commande non reconnue (ignorée).");
      }
      waitBody = false; // traiter une seule ligne de corps
      lastHeader = "";
      continue;
    }

    if (cur.startsWith("+CMT:")) {
      // Exemple header : +CMT: "+33699....",,"25/10/21,15:10:00+08"
      lastHeader = cur;
      waitBody = true;        // la prochaine ligne sera le corps
      continue;
    }
  }
}
