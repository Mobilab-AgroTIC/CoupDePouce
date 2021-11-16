/* Ce code permet d'activer un Servo Moteur et appuie sur un bouton pour lancer la pompe d'irrigation
 * L'envoi du SMS "1" permet d'activer ce système.
 * Ce code nécessite un MKR1400 ainsi qu'un Servo moteur
 */

#include <MKRGSM.h>
#include <Servo.h>
#include "arduino_secrets.h" 

const char PINNUMBER[] = SECRET_PINNUMBER;
const char Alex[] = ALEX;
const char Denis[] = DENIS;
const char Max[] = MAX;
const char Simon[] = SIMON;

char senderNumber[20];

GSM gsmAccess;
GSM_SMS sms;
Servo myservo;

void setup() {
  turnServo(10);
  digitalWrite(LED_BUILTIN,LOW);
  Serial.begin(9600);
  bool connected = false;
  
  while (!connected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      connected = true;
    } else {
      Serial.println("probleme initialisation");
    }
  }
  Serial.println("GSM initialized");
  char iniMsg[] = "Arduino OK : envoyer 1 pour allumer la pompe";
  envoiSMS(Simon, iniMsg);  
  envoiSMS(Alex, iniMsg);
  envoiSMS(Max, iniMsg);
  envoiSMS(Denis, iniMsg);
  envoiSMS(Simon, iniMsg);

  blinkLed(LED_BUILTIN,500);
}

void loop() {
  if (sms.available()) {
    Serial.print("Message received from : ");
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    //Allumange demandé
    if (sms.peek() == '1') {
      Serial.println("Allumage demandé");
      turnServo(90);
      char txtMsg[] = "Allumage en cours...";
      envoiSMS(Alex, txtMsg);
      envoiSMS(Max, txtMsg);
      envoiSMS(Denis, txtMsg);
      envoiSMS(Simon, txtMsg);
    }

    sms.flush();
    Serial.println("MESSAGE DELETED");
  }  
}

/*
  Read input serial
 */
int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}

//Fonction Envoi de SMS
int envoiSMS (char numero[], char msg[]) {
  sms.beginSMS(numero);
  sms.print(msg);
  sms.endSMS();
}

//Fonction BlinkLed
int blinkLed (int led, int delai) {
  digitalWrite(led, HIGH);
  delay(delai);
  digitalWrite(led, LOW);
  delay(delai);
}

//Fonction active servo
int turnServo(int rotation) {
  myservo.attach(6);  //Servo sur la broche 6
  delay(100);
  myservo.write(rotation);
  delay(1000);
  myservo.write(0);
  delay(500);
  myservo.detach();
}
