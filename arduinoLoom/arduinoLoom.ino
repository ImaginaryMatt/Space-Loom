#include <Arduino.h>
#include <Servo.h>
#include <AccelStepper.h>
#include <IRremote.hpp>

// Define pins for IR receiver
const int IR_PIN = 2;

// -------- Motor 1 --------
const int STEP1 = 48;
const int DIR1  = 47;

// -------- Motor 2 --------
const int STEP2 = 44;
const int DIR2  = 43;

// -------- Motor 3 --------
const int STEP3 = 40;
const int DIR3  = 39;

// -------- Motor 4 --------
const int STEP4 = 36;
const int DIR4  = 35;

// Define Position Constants for Motors
const int Open = 180;
const int Close = 20;
const int BeatUpSpeed = 500;
const int BeatUpDist = 2000;
const int RapierSpeed = 500;
const int RapierOut = 1600;
const int RapierIn = 1900;
const int RapierFull = 3400;

// Create stepper objects
AccelStepper stepper1(AccelStepper::DRIVER, STEP1, DIR1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP2, DIR2);
AccelStepper stepper3(AccelStepper::DRIVER, STEP3, DIR3);
AccelStepper stepper4(AccelStepper::DRIVER, STEP4, DIR4);

// Create servo objects
Servo ServoRapier;
Servo ServoClose;
Servo ServoFar;

const int SERVO_RAPIER_PIN = 9;
const int SERVO_CLOSE_PIN = 5;
const int SERVO_FAR_PIN = 7;

int readIrRemote();

void setup() {
  Serial.begin(115200);
  Serial.println("Serial Port Ready");

  stepper1.setMaxSpeed(1000);
  stepper2.setMaxSpeed(1000);
  stepper3.setMaxSpeed(1000);
  stepper4.setMaxSpeed(1000);

  stepper1.setSpeed(0);
  stepper2.setSpeed(0);
  stepper3.setSpeed(0);
  stepper4.setSpeed(0);
  Serial.println("Stepper Motors Ready");

  ServoRapier.attach(SERVO_RAPIER_PIN);
  ServoClose.attach(SERVO_CLOSE_PIN);
  ServoFar.attach(SERVO_FAR_PIN);

  ServoRapier.write(0);
  ServoClose.write(0);
  ServoFar.write(0);
  Serial.println("Servo control ready.");

  IrReceiver.begin(IR_PIN, DISABLE_LED_FEEDBACK);
  Serial.println("IR Receiver Ready");
}

void loop() {
  int button = readIrRemote();
  Serial.println(button);
  switch (button) {
    case 8:
    // Open Rapier Servo
      ServoRapier.write(Open);
      break;

    case 66:
    // Close Rapier Servo
      ServoRapier.write(Close);
      break;

    case 28:
    // Opern Close Servo
      ServoClose.write(Open);
      break;

    case 82:
    // Close Close Servo
      ServoClose.write(Close);
      break;

    case 90:
    // Open Far Servo
      ServoFar.write(Open);
      break;

    case 74:
    // Close Far Servo
      ServoFar.write(Close);
      break;

    case 22: {
      // Rapier Full Out
      unsigned long startTime = millis();
      stepper3.setSpeed(RapierSpeed);
      while (millis() - startTime < RapierFull) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
    }

    case 12: {
      // Rapier Full In
      unsigned long startTime = millis();
      stepper3.setSpeed(-RapierSpeed);
      while (millis() - startTime < RapierFull) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
    }

    case 25: {
      // Rapier Out to Home
      unsigned long startTime = millis();
      stepper3.setSpeed(-RapierSpeed);
      while (millis() - startTime < RapierOut) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
    }

    case 24: {
      // Rapier In to Home
      unsigned long startTime = millis();
      stepper3.setSpeed(RapierSpeed);
      while (millis() - startTime < RapierIn) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
    }

    case 13: {
      // Rapier Home to Out
      unsigned long startTime = millis();
      stepper3.setSpeed(RapierSpeed);
      while (millis() - startTime < RapierOut) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
    }

    case 94: {
      // Rapier Home to In
      unsigned long startTime = millis();
      stepper3.setSpeed(-RapierSpeed);
      while (millis() - startTime < RapierIn) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
    }

    case 64:
      // Continuous Operation
      break;

    case 68:
      // Pause Continuous Operation - Return to Home
      break;

    case 71:
      stepper1.setSpeed(0);
      stepper2.setSpeed(0);
      stepper3.setSpeed(0);
      stepper4.setSpeed(0);
      stepper1.runSpeed();
      stepper2.runSpeed();
      stepper3.runSpeed();
      stepper4.runSpeed();
      break;

    default:
      break;
  }
}

int readIrRemote() {
  if (IrReceiver.decode()) {
    int value = IrReceiver.decodedIRData.command;
    IrReceiver.resume();
    return value;
  }
  return -1;
}
