// Arduino Loom Code
#include <Arduino.h>
#include <Servo.h>
#include <AccelStepper.h>

// // Define pins for 4 stepper motors
// #define STEP_PIN 2
// #define DIR_PIN 3
// #define ENABLE_PIN 4


// // Define pins for servo motor
// #define _RAPIER 5

// -------- Motor 1 --------
const int STEP1 = 48;
const int DIR1  = 47;

// -------- Motor 2 --------
const int STEP2 = 44;
const int DIR2  = 43;

// -------- Motor 3 --------
const int STEP3 = 40;
const int DIR3 = 39;

// -------- Motor 4 --------
const int STEP4 = 36;
const int DIR4 = 35;

// Create stepper objects
AccelStepper stepper1(AccelStepper::DRIVER, STEP1, DIR1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP2, DIR2);
AccelStepper stepper3(AccelStepper::DRIVER, STEP3, DIR3);
AccelStepper stepper4(AccelStepper::DRIVER, STEP4, DIR4);

// Create servo object
Servo ServoRapier;
Servo ServoClose;
Servo ServoFar;

// Predeclaration of fuctions




const int SERVO_RAPIER_PIN = 9;   // Signal wire connected to pin 9
const int SERVO_CLOSE_PIN = 10;  // Signal wire connected to pin 10
const int SERVO_FAR_PIN = 11;    // Signal wire connected to pin 11
String inputString = "";
bool inputComplete = false;


void setup() {
  Serial.begin(115200);

  stepper1.setMaxSpeed(1000);
  stepper2.setMaxSpeed(1000);
  stepper3.setMaxSpeed(1000);
  stepper4.setMaxSpeed(1000);

  stepper1.setSpeed(0);
  stepper2.setSpeed(0);
  stepper3.setSpeed(0);
  stepper4.setSpeed(0);

  Serial.println("Stepper Motors Ready");

  // Initialize servos
 ServoRapier.attach(SERVO_RAPIER_PIN);
 ServoClose.attach(SERVO_CLOSE_PIN);
 ServoFar.attach(SERVO_FAR_PIN);

  Serial.println("Servo control ready.");
  Serial.println("Enter an angle from 0 to 180, then press Enter:");
  
  
 ServoRapier.write(0); // Start at 0 degrees
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    // -------- Forward --------
    if (c == 'f') {
      stepper3.setSpeed(-500);
      stepper4.setSpeed(500);
      Serial.println("Forward");
    }

    // -------- Backward --------
    else if (c == 'b') {
      stepper3.setSpeed(500);
      stepper4.setSpeed(-500);
      Serial.println("Backward");
    }

    // -------- Stop --------
    else if (c == 's') {
      stepper3.setSpeed(0);
      stepper4.setSpeed(0);
      Serial.println("Stop");
    }

    // -------- Run both for 1 second --------
    else if (c == 't') {
      Serial.println("Run both motors");

      stepper1.setSpeed(500);
      stepper2.setSpeed(500);

      unsigned long startTime = millis();
      while (millis() - startTime < 2800) {
        stepper1.runSpeed();
        stepper2.runSpeed();
      }

      stepper1.setSpeed(0);
      stepper2.setSpeed(0);

      Serial.println("Done");
    }
    else if (c == 'r') {
      Serial.println("Run both motors");

      stepper1.setSpeed(-500);
      stepper2.setSpeed(-500);

      unsigned long startTime = millis();
      while (millis() - startTime < 2800) {
        stepper1.runSpeed();
        stepper2.runSpeed();
      }

      stepper1.setSpeed(0);
      stepper2.setSpeed(0);

      Serial.println("Done");
    }
  }

  // Always run both motors
  stepper1.runSpeed();
  stepper2.runSpeed();
  stepper3.runSpeed();
  stepper4.runSpeed();
}







void loop() {
  if (inputComplete) {
    inputString.trim();  // Remove spaces/newlines

    if (inputString.length() > 0) {
      int angle = inputString.toInt();

      if (angle >= 0 && angle <= 180) {
     ServoRapier.write(angle);
        Serial.print("Moved servo to: ");
        Serial.print(angle);
        Serial.println(" degrees");
      } else {
        Serial.println("Invalid angle. Enter a value from 0 to 180.");
      }
    }

    inputString = "";
    inputComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();

    if (inChar == '\n') {
      inputComplete = true;
    } else if (inChar != '\r') {
      inputString += inChar;
    }
  }
}
