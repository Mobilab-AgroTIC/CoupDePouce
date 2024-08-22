/*
  CoupDePouce.ino v3.1

  Activer un moteur a distance via l'envoi de SMS. 
  Le montage nécessite  : 
  - une carte Arduino MKRWAN GSM 1400
  - un moteur "pas à pas" 28byj-48 avec une carte associée "ULN2003 driver"

  Activate a stepmoter through SMS Message. 
  Circuit needs : 
  - An Arduino MKRWANGSM 1400 
  - A stepper motor 28byj-48 with ULN2003 driver

  The circuit:
    
    ** stepper <-> Arduino
    
    ** "-"   <->   pin GND
    ** "5V"   <->   pin VCC

    ** IN1   <->   pin 2
    ** IN2   <->   pin 3
    ** IN3   <->   pin 4
    ** IN4   <->   pin 5
    

  Createad 25 March 2019
  modified 01 April 2022

  by Simon Moinard

  This example code is in the public domain.

*/


#include <MKRGSM.h>
#include "arduino_secrets.h"

#define STEPPER_1_PIN_1 6
#define STEPPER_1_PIN_2 7
#define STEPPER_1_PIN_3 8
#define STEPPER_1_PIN_4 9


const char PINNUMBER[] = CODE_PIN ;
char num1[] = numero1;
char num2[] = numero2;

char initMsg[] = "CoupDePouce operationnel. Envoyez 1 pour allumer le moteur !";
char msg1[] = "Allumage du bouton 1";

char senderNumber[20];
int step_number = 0;

int IN_1 ;
int IN_2 ;
int IN_3 ;
int IN_4 ;

GSM gsmAccess;
GSM_SMS sms;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(STEPPER_1_PIN_1, OUTPUT);
  pinMode(STEPPER_1_PIN_2, OUTPUT);
  pinMode(STEPPER_1_PIN_3, OUTPUT);
  pinMode(STEPPER_1_PIN_4, OUTPUT);

  stepper(1,true,10);
  stepper(1,false,10);
  
  shutDownStepper();

  bool connected = false;
  
  while (!connected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      connected = true;
    } else {
      Serial.println("probleme lors de l'initialisation GSM. La carte SIM est-elle bien en place ? Le code PIN est-t-il bien rempli dans le fichier arduino_secrets.h ? ");
    }
  }
  Serial.println("GSM intilialisé");

  sms.beginSMS(num1);
  sms.print(initMsg);
  sms.endSMS();
  delay(1500);
  sms.beginSMS(num2);
  sms.print(initMsg);
  sms.endSMS();

  blinkLed(LED_BUILTIN,500);

}

void loop() {
  
  if (sms.available()) {
    Serial.print("Message reçu de : ");
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    //Activation du moteur 1
    if (sms.peek() == '#') {
      Serial.println("Moteur 1 demandé");
      stepper(1,false,500);
      stepper(1,true,500);
      sms.beginSMS(senderNumber);
      sms.print(msg1);
      sms.endSMS();
    }
   
    
    sms.flush();
    Serial.println("MESSAGE EFFACÉ");
  }  

}

void stepper(int motor, bool dir, int rotation){

  IN_1 = STEPPER_1_PIN_1;
  IN_2 = STEPPER_1_PIN_2;
  IN_3 = STEPPER_1_PIN_3;
  IN_4 = STEPPER_1_PIN_4;

    
  for (int i = 0; i <= rotation; i++) {
    if(dir){
      switch(step_number){
        case 0:
          digitalWrite(IN_1, HIGH);
          digitalWrite(IN_2, LOW);
          digitalWrite(IN_3, LOW);
          digitalWrite(IN_4, LOW);
        break;
        case 1:
          digitalWrite(IN_1, LOW);
          digitalWrite(IN_2, HIGH);
          digitalWrite(IN_3, LOW);
          digitalWrite(IN_4, LOW);
        break;
        case 2:
          digitalWrite(IN_1, LOW);
          digitalWrite(IN_2, LOW);
          digitalWrite(IN_3, HIGH);
          digitalWrite(IN_4, LOW);
          break;
        case 3:
          digitalWrite(IN_1, LOW);
          digitalWrite(IN_2, LOW);
          digitalWrite(IN_3, LOW);
          digitalWrite(IN_4, HIGH);
          break;
      } 
    }
    else{
      switch(step_number){
        case 0:
          digitalWrite(IN_1, LOW);
          digitalWrite(IN_2, LOW);
          digitalWrite(IN_3, LOW);
          digitalWrite(IN_4, HIGH);
          break;
        case 1:
          digitalWrite(IN_1, LOW);
          digitalWrite(IN_2, LOW);
          digitalWrite(IN_3, HIGH);
          digitalWrite(IN_4, LOW);
          break;
        case 2:
          digitalWrite(IN_1, LOW);
          digitalWrite(IN_2, HIGH);
          digitalWrite(IN_3, LOW);
          digitalWrite(IN_4, LOW);
          break;
        case 3:
          digitalWrite(IN_1, HIGH);
          digitalWrite(IN_2, LOW);
          digitalWrite(IN_3, LOW);
          digitalWrite(IN_4, LOW);
      } 
    }
    
    step_number++;
    
    if(step_number > 3){
      step_number = 0;
    }
    delay(20);
  }
  shutDownStepper();
}

void shutDownStepper() {
  digitalWrite(STEPPER_1_PIN_1, LOW);
  digitalWrite(STEPPER_1_PIN_2, LOW);
  digitalWrite(STEPPER_1_PIN_3, LOW);
  digitalWrite(STEPPER_1_PIN_4, LOW);  

}

//Fonction BlinkLed
void blinkLed (int led, int delai) {
  digitalWrite(led, HIGH);
  delay(delai);
  digitalWrite(led, LOW);
  delay(delai);
}
