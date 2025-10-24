#include <Arduino.h>

// === Broches (modifie l'ordre si ton IN1..IN4 n'est pas 39,40,41,42) ===
const int PIN_IN1 = 39;
const int PIN_IN2 = 40;
const int PIN_IN3 = 41;
const int PIN_IN4 = 42;

// === Séquence demi-pas (8 états) : 4096 pas / tour sur 28BYJ-48 ===
const uint8_t HALFSTEP_SEQ[8][4] = {
  {1,0,0,0},
  {1,1,0,0},
  {0,1,0,0},
  {0,1,1,0},
  {0,0,1,0},
  {0,0,1,1},
  {0,0,0,1},
  {1,0,0,1}
};

const int STEPS_PER_REV = 4096;   // demi-pas
float rpm = 10.0;                 // vitesse cible
unsigned long stepDelayUs = 0;    // µs entre demi-pas (calculé)

int seqIndex = 0;                 // index 0..7

void outPhase(const uint8_t p[4]) {
  digitalWrite(PIN_IN1, p[0]);
  digitalWrite(PIN_IN2, p[1]);
  digitalWrite(PIN_IN3, p[2]);
  digitalWrite(PIN_IN4, p[3]);
}

void stepCW() {                   // sens horaire
  seqIndex = (seqIndex + 1) & 7;
  outPhase(HALFSTEP_SEQ[seqIndex]);
  delayMicroseconds(stepDelayUs);
}
void stepCCW() {                  // sens anti-horaire
  seqIndex = (seqIndex + 7) & 7;
  outPhase(HALFSTEP_SEQ[seqIndex]);
  delayMicroseconds(stepDelayUs);
}

void setRPM(float newRPM) {
  rpm = newRPM;
  // 1 tour = STEPS_PER_REV demi-pas, donc pas/s = rpm * steps/60
  float stepsPerSec = (rpm * STEPS_PER_REV) / 60.0f;
  if (stepsPerSec < 1.0f) stepsPerSec = 1.0f; // garde-fou
  stepDelayUs = (unsigned long)(1e6 / stepsPerSec);
}

void releaseCoils() { // optionnel : coupe le courant (évite de chauffer, moins de couple)
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);
}

void setup() {
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  setRPM(10.0);                   // 10 RPM par défaut
  outPhase(HALFSTEP_SEQ[seqIndex]); // position initiale
}

void loop() {
  // Exemple : 1 tour CW, pause, 1 tour CCW, pause
  for (int i = 0; i < STEPS_PER_REV; ++i) stepCW();
  releaseCoils(); delay(300);

  for (int i = 0; i < STEPS_PER_REV; ++i) stepCCW();
  releaseCoils(); delay(500);
}
