#include <Arduino.h>
#include <ESP32Servo.h>

// ================== CONFIG LIGNE / SIM ==================
// Mets ton code PIN entre guillemets (laisser "" si pas de PIN)
#define SIM_PIN "0000"

// Numéros destinataires en E.164 (ex: +336XXXXXXXX)
static const char* DESTS[] = {
  "+336xxxxxxxx",
  "+336xxxxxxxx",
};
const size_t N_DESTS = sizeof(DESTS) / sizeof(DESTS[0]);

// ================== MODEM A7670E (UART2) ==================
HardwareSerial SerialAT(2);
#define IO_RXD2         47
#define IO_TXD2         48
#define IO_GSM_PWRKEY    4

// ================== SERVOS ==================
enum ServoType { ROTATE = 0, PUSH = 1 };
enum clockWise { ANTIHORAIRE = 0, HORAIRE = 1 };

struct ServoConf {
  char      id;          // 'A', 'B', ...
  int       pin;         // broche ESP32-S3
  ServoType type;        // ROTATE ou PUSH
  int       angleMax;    // 0..180
  clockWise clockwise;   // true: sens horaire, false: anti-horaire
};

// --- AJUSTE ICI TES SERVOS (ex: A/B par défaut) ---
static ServoConf SERVOS[] = {
  //  id, Broche, type,    angleMax, Sens_Horaire
  { 'A', 40,  ROTATE,  90,       HORAIRE     },
  { 'B', 41,  PUSH,    60,       ANTIHORAIRE },
  // { 'C', 42,  ROTATE, 120,      HORAIRE  },  // exemple
};

const size_t N_SERVOS = sizeof(SERVOS) / sizeof(SERVOS[0]);

// Moteur servo unique réutilisé (un à la fois)
Servo g_servo;

// ================== OUTILS AT ==================
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

static bool waitFor(const char* token, uint32_t timeout_ms = 3000) {
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

static void modem_power_on() {
  pinMode(IO_GSM_PWRKEY, OUTPUT);
  digitalWrite(IO_GSM_PWRKEY, HIGH);
  delay(100);
  digitalWrite(IO_GSM_PWRKEY, LOW);   // impulsion ~1.2s
  delay(1200);
  digitalWrite(IO_GSM_PWRKEY, HIGH);
  delay(3000);
}

static bool waitNetwork(uint32_t max_ms = 60000) {
  uint32_t t0 = millis();
  while (millis() - t0 < max_ms) {
    sendAT("AT+CEREG?");
    String buf; uint32_t t1 = millis();
    while (millis() - t1 < 1000) { while (SerialAT.available()) buf += (char)SerialAT.read(); }
    Serial.print(buf);
    if (buf.indexOf(",1") >= 0 || buf.indexOf(",5") >= 0) return true; // registered / roaming
    delay(800);
  }
  return false;
}

// ================== SMS ==================
static bool sendSMS(const String& text, const char* number) {
  purgeInput();
  Serial.printf("Envoi SMS à %s : \"%s\"\n", number, text.c_str());

  SerialAT.print("AT+CMGS=\"");
  SerialAT.print(number);
  SerialAT.print("\"\r\n");

  if (!waitFor(">", 5000)) { Serial.println("! Prompt '>' non reçu"); return false; }

  SerialAT.print(text);
  SerialAT.write(0x1A); // Ctrl-Z

  if (!waitFor("OK", 15000)) { Serial.println("! OK final non reçu"); return false; }
  return true;
}

static void sendSMSAll(const String& text) {
  for (size_t i=0; i<N_DESTS; i++) {
    bool ok = sendSMS(text, DESTS[i]);
    Serial.printf("[SMS -> %s] %s\n", DESTS[i], ok ? "OK" : "ECHEC");
    delay(400);
  }
}

static bool isKnownDest(const String& number) {
  for (size_t i=0; i<N_DESTS; i++) {
    if (number == DESTS[i]) return true;
  }
  return false;
}

static void sendSMSAllPlusSender(const String& text, const String& sender) {
  sendSMSAll(text);
  if (sender.length() && !isKnownDest(sender)) {
    (void)sendSMS(text, sender.c_str());
  }
}

// ================== UTILS PARSING ==================
static int findServoIndexById(char id) {
  char up = toupper((unsigned char)id);
  for (size_t i=0; i<N_SERVOS; i++) if (toupper(SERVOS[i].id) == up) return (int)i;
  return -1;
}

static String extractNumberFromCMT(const String& header) {
  // Exemple header: +CMT: "+33699....",,"25/10/21,15:10:00+08"
  int p1 = header.indexOf('\"');
  if (p1 < 0) return "";
  int p2 = header.indexOf('\"', p1+1);
  if (p2 < 0) return "";
  return header.substring(p1+1, p2);
}

static String buildHelpText() {
  String s = "CoupDePouce operationnel.\n";
  s += "Commandes:\n";
  for (size_t i=0; i<N_SERVOS; i++) {
    const ServoConf& c = SERVOS[i];
    s += String(c.id) + "1: ";
    s += (c.type==ROTATE) ? "ON (0->" : "PUSH (0->";
    s += String(c.angleMax) + (c.type==PUSH? "->0)\n" : ")\n");
    if (c.type==ROTATE) s += String(c.id) + "0: OFF (" + String(c.angleMax) + "->0)\n";
  }
  s += "HELP: ce message.";
  return s;
}

// ================== SERVO MOUVEMENT ==================
static void attachForPin(int pin) {
  g_servo.attach(pin, 500, 2500); // PWM 50 Hz (~500-2500µs)
}

static void detachAndFloat(int pin) {
  g_servo.detach();
  pinMode(pin, INPUT_PULLDOWN);
}

static void sweep(int from, int to, int stepDelayMs=12) {
  if (from <= to) {
    for (int a=from; a<=to; a++) { g_servo.write(a); delay(stepDelayMs); }
  } else {
    for (int a=from; a>=to; a--) { g_servo.write(a); delay(stepDelayMs); }
  }
}

// Convertit 0/AngleMax logiques vers angles physiques selon sens
static int physAngle(int logicalAngle, const ServoConf& c) {
  // logique: 0 = OFF, angleMax = ON
  if (c.clockwise) return logicalAngle;
  // anti-horaire: on inverse autour de 90? Non: on suppose map simple 0..180 -> 180..0
  return 180 - logicalAngle;
}

static void action_ON(const ServoConf& c) {
  attachForPin(c.pin);
  int offA = physAngle(0, c);
  int onA  = physAngle(c.angleMax, c);
  sweep(offA, onA);
  if (c.type == PUSH) {
    delay(250);
    sweep(onA, offA);
  }
  delay(200);
  detachAndFloat(c.pin);
}

static void action_OFF(const ServoConf& c) {
  if (c.type == PUSH) return; // non applicable
  attachForPin(c.pin);
  int offA = physAngle(0, c);
  int onA  = physAngle(c.angleMax, c);
  sweep(onA, offA);
  delay(200);
  detachAndFloat(c.pin);
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  delay(1200);
  Serial.println("\n[CoupDePouce - ESP32-S3 + A7670E | Multi-servos SMS]");

  SerialAT.begin(115200, SERIAL_8N1, IO_RXD2, IO_TXD2);
  SerialAT.setRxBufferSize(4096);

  // Petit clignotement de vie via log
  modem_power_on();

  // Sync modem
  for (int i=0; i<5; i++) { sendAT("AT"); if (waitFor("OK", 800)) break; delay(150); }

  // Hygiène
  sendAT("ATE0");       // echo off
  sendAT("AT+CMEE=2");  // erreurs verbeuses

  // PIN SIM ?
  sendAT("AT+CPIN?");
  {
    String r; uint32_t t=millis();
    while (millis()-t < 1500) { while (SerialAT.available()) r += (char)SerialAT.read(); }
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
      Serial.println("SIM bloquee (PUK)");
    } else if (r.indexOf("NOT INSERTED") >= 0) {
      Serial.println("Pas de SIM");
    }
  }

  Serial.println("Attente reseau...");
  if (!waitNetwork(60000)) Serial.println("Reseau KO (timeout)");
  else Serial.println("Reseau OK");

  // Mode SMS texte, alphabet GSM
  sendAT("AT+CMGF=1");
  sendAT("AT+CSCS=\"GSM\"");

  // *** IMPORTANT : URC direct, pas de stockage des nouveaux SMS
  sendAT("AT+CNMI=2,2,0,0,0");

  // *** Purge des anciens SMS stockés (SM & ME) avant d'écouter
  sendAT("AT+CPMS=\"SM\",\"SM\",\"SM\"");
  sendAT("AT+CMGD=1,4");
  sendAT("AT+CPMS=\"ME\",\"ME\",\"ME\"");
  sendAT("AT+CMGD=1,4");

  // Message d'accueil + petit mouvement de test pour chaque servo
  String help = buildHelpText();
  sendSMSAll(help);

  for (size_t i=0; i<N_SERVOS; i++) {
    const ServoConf& c = SERVOS[i];
    attachForPin(c.pin);
    int offA = physAngle(0, c);
    int onA  = physAngle(min(c.angleMax, 20), c);
    sweep(offA, onA, 8);
    sweep(onA, offA, 8);
    detachAndFloat(c.pin);
    delay(120);
  }
}

// ================== LOOP (URC only) ==================
void loop() {
  static String line;
  static bool   waitBody   = false;
  static String lastHeader;

  while (SerialAT.available()) {
    char c = (char)SerialAT.read();
    if (c == '\r') continue;
    if (c != '\n') { line += c; continue; }

    // Fin de ligne
    String cur = line; line = "";
    if (cur.length() == 0) continue;

    Serial.print("[URC] "); Serial.println(cur);

    if (waitBody) {
      String body = cur;
      body.replace("\"", "");
      body.trim();
      String sender = extractNumberFromCMT(lastHeader);

      Serial.print("SMS recu de "); Serial.print(sender); Serial.print(" : "); Serial.println(body);

      // Normalisation
      body.toUpperCase();

      // HELP ?
      if (body == "HELP" || body == "H") {
        sendSMSAllPlusSender(buildHelpText(), sender);
        waitBody=false; lastHeader=""; continue;
      }

      // Forme X0 / X1 attendue
      bool valid = false;
      if (body.length() == 2) {
        char sid = body.charAt(0);
        char cmd = body.charAt(1);
        int idx = findServoIndexById(sid);
        if (idx >= 0 && (cmd=='0' || cmd=='1')) {
          const ServoConf& conf = SERVOS[idx];
          if (cmd == '1') {
            valid = true;
            sendSMSAllPlusSender(String("OK ")+conf.id+"1", sender);
            action_ON(conf);
          } else { // '0'
            if (conf.type == ROTATE) {
              valid = true;
              sendSMSAllPlusSender(String("OK ")+conf.id+"0", sender);
              action_OFF(conf);
            } else {
              // PUSH n'a pas de OFF
              sendSMSAllPlusSender(String("Commande ")+conf.id+"0 non applicable (PUSH).", sender);
            }
          }
        }
      }

      if (!valid && body.length() == 1 && (body[0] >= 'A' && body[0] <= 'Z')) {
        // Permettre 'A' seul => ON (alias pratique)
        int idx = findServoIndexById(body[0]);
        if (idx >= 0) {
          const ServoConf& conf = SERVOS[idx];
          sendSMSAllPlusSender(String("OK ")+conf.id+"1", sender);
          action_ON(conf);
          valid = true;
        }
      }

      if (!valid) {
        sendSMSAllPlusSender("Commande non reconnue. Envoie 'HELP'.", sender);
      }

      waitBody = false;
      lastHeader = "";
      continue;
    }

    if (cur.startsWith("+CMT:")) {
      lastHeader = cur;
      waitBody   = true;   // prochaine ligne = corps
      continue;
    }
  }
}
