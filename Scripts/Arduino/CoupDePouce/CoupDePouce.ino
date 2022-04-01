/*
  CoupDePouce.ino v3.1

  Activate a stepmoter through SMS Message. Circuit needs an Arduino MKRWANGSM 1400 and a stepper motor 28byj-48 with ULN2003 driver
  Activer un moteur a distance via l'envoi de SMS. Le montage nécessite une carte Arduino MKRWAN GSM 1400 ainsi qu'un moteur "pas à pas" 28byj-48 avec une carte associée "ULN2003 driver"

  The circuit:
    
    ** stepper <-> Arduino
    
    ** "-"   <->   pin GND
    ** "+"   <->   pin VCC

    ** IN1   <->   pin 6
    ** IN2   <->   pin 7
    ** IN3   <->   pin 8
    ** IN4   <->   pin 9

  Createad 25 March 2019
  modified 01 April 2022

  by Simon Moinard

  This example code is in the public domain.

*/

#include <MKRGSM.h>
#include <Servo.h>
#include "arduino_secrets.h" 

const char PINNUMBER[] = SECRET_PINNUMBER;
const char num1[] = num1;
const char num2[] = num2;
//Vous pouvez enlever les // dans la ligne suivante pour ajouter un 3ème numéro
//const char num3[] = num3;

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
  char iniMsg[] = "CoupDePouce opérationnel. Envoyez 1 pour allumer le moteur 1, ou 2 pour le moteur 2 !";
  
  envoiSMS(num1, iniMsg);  
  envoiSMS(num2, iniMsg);
//envoiSMS(num3, iniMsg);
  
  blinkLed(LED_BUILTIN,500);
}

void loop() {
  if (sms.available()) {
    Serial.print("Message reçu de : ");
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    //Activation du moteur 1
    if (sms.peek() == '1') {
      Serial.println("Moteur 1 demandé");
      turnServo(90);
      char txtMsg[] = "Le moteur 1 s'active...";
      envoiSMS(num1, txtMsg);
      envoiSMS(num2, txtMsg);
    }
    
    //Activation du moteur 2
     if (sms.peek() == '2') {
      Serial.println("Moteur 2 demandé");
      turnServo(90);
      char txtMsg[] = "Le moteur 1 s'active...";
      envoiSMS(num1, txtMsg);
      envoiSMS(num2, txtMsg);
    }
   
    
    sms.flush();
    Serial.println("MESSAGE EFFACÉ");
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

void OneStep(bool dir){
  if(dir){
    switch(step_number){
      case 0:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
      break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
      break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
    } 
  }
  else{
    switch(step_number){
      case 0:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
    } 
  }
  
  void OneStep(bool dir){
  if(dir){
    switch(step_number){
      case 0:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
      break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
      break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
    } 
  }
  else{
    switch(step_number){
      case 0:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
    } 
  }
