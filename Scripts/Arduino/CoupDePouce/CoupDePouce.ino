/*
  CoupDePouce.ino

  Activate a stepmoter through SMS Message. Circuit needs an Arduino MKRWANGSM 1400 and a stepper motor 28byj-48 with ULN2003 driver
  Activer un moteur a distance via l'envoi de SMS. Le montage nécessite une carte Arduino MKRWAN GSM 1400 ainsi qu'un moteur pas à pas 28byj-48 with ULN2003 driver

  The circuit:

** - <-> pin GND
** + <-> pin VCC

** IN1 <-> pin 6
** IN2 <-> pin 7
** IN3 <-> pin 8
** IN4 <-> pin 9

  Createad 25 March 2019
  modified 01 April 2022
  v3.1
  by Tom Igoe

  This example code is in the public domain.

*/

#include <MKRGSM.h>
#include <Servo.h>
#include "arduino_secrets.h" 

const char PINNUMBER[] = SECRET_PINNUMBER;
const char num1[] = num1;
const char num2[] = num2;

//Rajoutez une ligne ici pour rajouter un numéro de téléphone : const char num3[] = num3;

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
      Serial.println("probleme lors de l'initialisation GSM. La carte SIM est-elle bien en place ? Le code PIN est-t-il bien rempli dans le fichier arduino_secrets.h ? ");
    }
  }
  Serial.println("GSM intilialisé");
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
