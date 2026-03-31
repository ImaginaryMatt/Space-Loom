// Arduino Loom Code
#include <Arduino.h>
#include <Servo.h>
#include <AccelStepper.h>
#include <IRremote.h>

// Define pins for IR receiver
const int IR_PIN =  2; // IR receiver connected to pin 52

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

// Define Position Constants for Motors
const int Open = 180;
const int Close = 20;
const int BeatUpSpeed = 500;
const int BeatUpDist = 2000;
const int RapierSpeed = 1000;
const int RapierOut = 1600;
const int RapierIn = 1800;
const int RapierFull = 3400;

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
const int SERVO_CLOSE_PIN = 5;  // Signal wire connected to pin 10
const int SERVO_FAR_PIN = 7;    // Signal wire connected to pin 11
String inputString = "";
bool inputComplete = false;


void setup() {
  // Initialize Serial Port
  Serial.begin(115200);
  Serial.println("Serial Port Ready");

  // Initialize Servo Motors
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
  ServoRapier.write(0); // Start at 0 degrees
  ServoClose.write(0); // Start at 0 degrees
  ServoFar.write(0); // Start at 0 degrees
  Serial.println("Servo control ready.");
  
  // Declare IR Receiver
  IrReceiver.begin(IR_PIN, DISABLE_LED_FEEDBACK);
  // Initialize IR receiver
  pinMode(IR_PIN, INPUT);
  Serial.println("IR Receiver Ready");
  irrecv.enableIRIn(); // Start the IR receiver
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

}

void loop() {
  // Read IR remote input and decide which motor to control based on the value received
  int button = readIrRemote();
  switch (button) {
      case 8: // Open Rapier Grabber
      // Open Servo Rapier
      ServoRapier.write(Open);
      break;
      case 66:
      // Close Servo Rapier
      ServoRapier.write(Close);
      break;
      case 28:
      // Open Near Grabber
      ServoRapier.write(Open);
      break;
      case 82;
      // Close Near Grabber
      ServoClose.write(Close);
      break;
      case 90:
      // Open Far Grabber
      ServoClose.write(Open);
      break;
      case 74;
      // Close Far Grabber
      ServoFar.write(Close);
      break;
      case 22:
      // Rapier Stepper Motor Out Full
      stepper3.setSpeed(RapierSpeed);
      unsigned long startTime = millis();
      while (millis() - startTime < RapierFull) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
      case 12;
      // Rapier Stepper Motor In Full
      stepper3.setSpeed(-RapierSpeed);
      unsigned long startTime = millis();
      while (millis() - startTime < RapierFull) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
      case 25:
      // Rapier OUT -> Home
      stepper3.setSpeed(-RapierSpeed);
      unsigned long startTime = millis();
      while (millis() - startTime < RapierOut) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
      case 24;
      // Rapier IN -> Home
      stepper3.setSpeed(RapierSpeed);
      unsigned long startTime = millis();
      while (millis() - startTime < RapierIn) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
      case 13:
      // Rapier Home -> OUT
      stepper3.setSpeed(RapierSpeed);
      unsigned long startTime = millis();
      while (millis() - startTime < RapierOut) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
      case 94;
      // Rapier Home -> IN
      stepper3.setSpeed(-RapierSpeed);
      unsigned long startTime = millis();
      while (millis() - startTime < RapierIn) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      break;
      case 64:
      // Continuous Operation
  
      break;
      case 68;
      // Pause Continuous Operation - Return to Home
  
      break;
      case 71:
      // EMERGENCY - Stop all Motors
      stepper1.setSpeed(0);
      stepper2.setSpeed(0);
      stepper3.setSpeed(0);
      stepper4.setSpeed(0);
      stepper1.runSpeed();
      stepper2.runSpeed();
      stepper3.runSpeed();
      stepper4.runSpeed();
      break;
      default;
      // Nothing right now
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
