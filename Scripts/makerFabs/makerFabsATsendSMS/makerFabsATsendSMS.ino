#include <Arduino.h>

HardwareSerial mySerial2(2);

#define IO_RXD2 47
#define IO_TXD2 48

#define IO_GSM_PWRKEY 4
#define IO_GSM_RST    5

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print(F("Hello! ESP32-S3 AT command V1.0 Test"));
  mySerial2.begin(115200,SERIAL_8N1, IO_RXD2, IO_TXD2);
  pinMode(IO_GSM_RST, OUTPUT);
  digitalWrite(IO_GSM_RST, LOW);
  
  pinMode(IO_GSM_PWRKEY, OUTPUT);
  digitalWrite(IO_GSM_PWRKEY, HIGH);
  delay(3000);
  digitalWrite(IO_GSM_PWRKEY, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
while (Serial.available() > 0) {
  int c = Serial.read();

  static bool tilde = false;
  if (!tilde && c == '~') { tilde = true; continue; }
  if (tilde) {
    tilde = false;
    if (c == 'Z' || c == 'z') {        // ~Z -> Ctrl-Z
      mySerial2.write(0x1A);
      Serial.println("[sent ^Z]");
      continue;
    }
  }

  mySerial2.write(c);
  yield();
}

  while (mySerial2.available() > 0) {
    Serial.write(mySerial2.read());
    yield();
  }
}
