/*
 SMS receiver

 This sketch, for the MKR GSM 1400 board, waits for a SMS message
 and displays it through the Serial port.

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * SIM card that can receive SMS messages

 created 25 Feb 2012
 by Javier Zorzano / TD
*/

// include the GSM library
#include <MKRGSM.h>
#include <Servo.h>

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = "0000";
const char num1[] = "+33699227482";
const char num2[] = "+33614421056";


Servo myservo;  // create servo object to control a servo
// initialize the library instances
GSM gsmAccess;
GSM_SMS sms;

// Array to hold the number a SMS is retreived from
char senderNumber[20];
int pos = 0;    // variable to store the servo position

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("-----------------------");
  Serial.println("CoupDePouce - servo 9g");

  // connection state
  bool connected = false;

  // Start GSM connection
  while (!connected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized");
  Serial.println("Waiting for messages");

    // send the message
  sms.beginSMS(num1);
  sms.print("CoupDePouce operationnel");
  sms.endSMS();
  Serial.println("\nmessage envoyé\n");
  delay(1000);
      // send the message
  sms.beginSMS(num2);
  sms.print("CoupDePouce operationnel");
  sms.endSMS();
  Serial.println("\nmessage envoyé\n");
  delay(1000);

}

void loop() {
  int c;

  // If there are any SMSs available()
  if (sms.available()) {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '1') {
      Serial.println("Allumage demandé");
      sms.flush();

      // send the message
      sms.beginSMS(num1);
      sms.print("allumage...");
      sms.endSMS();
      Serial.println("\nmessage envoyé\n");
      delay(500);
      // send the message
      sms.beginSMS(num2);
      sms.print("allumage...");
      sms.endSMS();
      Serial.println("\nmessage envoyé\n");
      delay(500);


      myservo.attach(2);  // attaches the servo on pin 9 to the servo object
      for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(30);                       // waits 15 ms for the servo to reach the position
      }
      for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(30);                       // waits 15 ms for the servo to reach the position
      }
      myservo.detach();  // attaches the servo on pin 9 to the servo object

    }

    // Read message bytes and print them
    while ((c = sms.read()) != -1) {
      Serial.print((char)c);
    }

    Serial.print("\nFin de message");

    // Delete message from modem memory
    sms.flush();
    Serial.println(", message effacé");
  }

  delay(1000);

}

