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
  // set control state to zero
  state = 0;
  // set autonomous step to zero
  step = -1;
  // initialize system as paused
  pause = 1;
  // initialize system as just completing an action
  cAflag = 1;

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

  // if in auto mode, don't allow random buttons to interrupt auto mode
  if button != 1 && step == -1{
    state = button;
  } 
  
  // allow emergency stop and pause to happen at any time
  if button == 71 || button == 68 {
    state = button;
  }

  // Main Control Switch Case
  switch (state) {
    case 8:
    // Open Rapier Servo
      ServoRapier.write(Open);

      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
      break;

    case 66:
    // Close Rapier Servo
      ServoRapier.write(Close);
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
      break;

    case 28:
    // Open Near Servo
      ServoClose.write(Open);
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
      break;

    case 82:
    // Close Near Servo
      ServoClose.write(Close);
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
      break;

    case 90:
    // Open Far Servo
      ServoFar.write(Open);
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
      break;

    case 74:
    // Close Far Servo
      ServoFar.write(Close);
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
      break;

    case 22: {
      // Rapier IN to Out
      unsigned long startTime = millis();
      stepper3.setSpeed(RapierSpeed);
      while (millis() - startTime < RapierFull) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
      break;
    }

    case 12: {
      // Rapier Out to In
      unsigned long startTime = millis();
      stepper3.setSpeed(-RapierSpeed);
      while (millis() - startTime < RapierFull) {
        stepper3.runSpeed();
      }
      stepper3.setSpeed(0);
      stepper3.runSpeed();
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
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
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
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
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
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
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
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
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
      break;
    }

    case 7:
      // Beat Up in to out
      unsigned long startTime = millis();
      stepper1.setSpeed(BeatUpSpeed);
      stepper2.setSpeed(BeatUpSpeed);
      while (millis() - startTime < BeatUpDist) {
        stepper1.runSpeed();
        stepper2.runSpeed();
      }
      stepper1.setSpeed(0);
      stepper2.setSpeed(0);
      stepper1.runSpeed();
      stepper2.runSpeed();
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
      break;

    case 9:
      // Beat Up out to in
      unsigned long startTime = millis();
      stepper1.setSpeed(-BeatUpSpeed);
      stepper2.setSpeed(-BeatUpSpeed);
      while (millis() - startTime < BeatUpDist) {
        stepper1.runSpeed();
        stepper2.runSpeed();
      }
      stepper1.setSpeed(0);
      stepper2.setSpeed(0);
      stepper1.runSpeed();
      stepper2.runSpeed();
      
      // check if auto play is on
      if play == 1 {
        state = 64;
        cAflag = 1;
      }
      break;

    case 64:
      // Continuous Operation
      // if paused is not set and not currently playing, reset pause, set to play, and reset step to zero
      if pause == 1 && play = 0 {
        pause = 0;
        play = 1;
        step = 0;
      }

      // if playing auto mode, increment step and set the cAflag to not complete
      if play == 1 {
        step = step+1;
        cAflag = 0;
      }

      // if user has not selected pause, and auto is at step 12, reset to step 2
      if pause == 0 && step == 12 {
        step = 2;
      }

      // if user has paused, and auto is at step 11, then set step to zero and play to zero
      if pause == 1 && step == 11 {
        step = 0;
        play = 0;
      }
      break;

    case 68:
      // Pause Continuous Operation - Finish steps and return to home
      pause = 1;
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

  // Auto Mode State Machine Switch case
if cAflag == 0; {
    switch (step) {
      case 0:
          // rapier In to Home
          state = 24;
          step = -1;
      break;
      case 1:
          // rapier Home to out
          state = 13;
      break;
      case 2:
          // rapier servo close
          state = 66;
      break;
      case 3:
          // rapier out to in
          state = 12;
      break;
      case 4:
          // beat up out to in
          state = 9;
      break;
      case 5:
          // near servo close
          state = 82;
      break;
      case 6:
          // far servo close
          state = 74;
      break;
      case 7:
          // rapier servo open
          state = 8;
      break;
      case 8:
          // beat up in to out
          state = 7;
      break;
      case 9:
          // near servo open
          state = 28;
      break;
      case 10:
          // far servo open
          state = 90;
      break;
      case 11:
          // rapier in -> out
          state = 22;
      break;
      default
      break;
    }
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
