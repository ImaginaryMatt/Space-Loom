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

// Define Position Constants for Motors / Servos
const int Open = 180;
const int Close = 20;

const int BeatUpSpeed = 500;
const int BeatUpDist = 1800;

const int RapierSpeed = 1000;
const int RapierOut = 3600;
const int RapierIn = 3600;
const int RapierFull = 3400;

// Unsure which stepper is where
// Create stepper objects
AccelStepper stepperFar(AccelStepper::DRIVER, STEP1, DIR1);
AccelStepper stepperNear(AccelStepper::DRIVER, STEP2, DIR2);
AccelStepper stepperRapier(AccelStepper::DRIVER, STEP3, DIR3);
AccelStepper stepper4(AccelStepper::DRIVER, STEP4, DIR4);

// Create servo objects
Servo ServoRapier;
Servo ServoNear;
Servo ServoFar;

const int SERVO_RAPIER_PIN = 9;
const int SERVO_NEAR_PIN = 5;
const int SERVO_FAR_PIN = 7;

// ---- Control flags ----
bool continuousMode = false;
bool pausedMode = false;

// ---- Optional tracking for home state ----
bool rapierIsHome = true;

// Function declarations
int readIrRemote();
void stopAllSteppers();
void runStepperForTime(AccelStepper &stepper, int speedValue, unsigned long durationMs);
void runTwoStepperForTime(AccelStepper &stepper1, AccelStepper &stepper2, int speedValue, unsigned long durationMs);
void rapierFullOut();
void rapierFullIn();
void rapierOutToHome();
void rapierInToHome();
void rapierOut();
void rapierIn();
void beatUpForward();
void beatUpBackward();
void returnSystemHome();
void runContinuousCycle();
void checkForStop();

void setup() {
  Serial.begin(115200);
  Serial.println("Serial Port Ready");

  stepperFar.setMaxSpeed(1000);
  stepperNear.setMaxSpeed(1000);
  stepperRapier.setMaxSpeed(1000);
  stepper4.setMaxSpeed(1000);

  stepperFar.setSpeed(0);
  stepperNear.setSpeed(0);
  stepperRapier.setSpeed(0);
  stepper4.setSpeed(0);
  Serial.println("Stepper Motors Ready");

  ServoRapier.attach(SERVO_RAPIER_PIN);
  ServoNear.attach(SERVO_NEAR_PIN);
  ServoFar.attach(SERVO_FAR_PIN);

  // Start at a known resting position
  ServoRapier.write(Close);
  ServoNear.write(Close);
  ServoFar.write(Close);
  Serial.println("Servo control ready.");

  IrReceiver.begin(IR_PIN, DISABLE_LED_FEEDBACK);
  Serial.println("IR Receiver Ready");
}

void loop() {
  int button = readIrRemote();

  if (button != -1) {
    Serial.print("IR Command: ");
    Serial.println(button);

    switch (button) {
      case 8: {
        // Open Rapier Servo
        ServoRapier.write(Open);
        break;
      }
      case 66: {
        // Close Rapier Servo
        ServoRapier.write(Close);
        break;
      }
      case 28: {
        // Open Close Servo
        ServoNear.write(Open);
        break;
      }
      case 82:{
        // Close Close Servo
        ServoNear.write(Close);
        break;
    }
      case 90: {
        // Open Far Servo
        ServoFar.write(Open);
        break;
      }
      case 74:{
        // Close Far Servo
        ServoFar.write(Close);
        break;
      }
      case 22: {
        // Rapier Full Out
        rapierFullOut();
        break;
      }
      case 12: {
        // Rapier Full In
        rapierFullIn();
        break;
      }
      case 25: {
        // Rapier Out to Home
        rapierOutToHome();
        break;
      }
      case 24: {
        // Rapier In to Home
        rapierInToHome();
        break;
      }
      case 13:{
        // Rapier Home to Out
        rapierOut();
        break;
      }
      case 94: {
        // Rapier Home to In
        rapierIn();
        break;
      }
      case 9: {
        beatUpForward();
        break;
      }
      case 7: {
        beatUpBackward();
        break;
      }
      case 64: {
        // Continuous Operation
        continuousMode = true;
        pausedMode = false;
        Serial.println("Continuous mode ENABLED");
        break;
      }
      case 68: {
        // Pause Continuous Operation - Return to Home
        continuousMode = false;
        pausedMode = true;
        Serial.println("Continuous mode PAUSED - returning home");
        returnSystemHome();
        pausedMode = false;
        break;
      }
      case 71: {
        // Emergency stop
        continuousMode = false;
        pausedMode = false;
        stopAllSteppers();
        Serial.println("Emergency stop");
        break;
      }
      default:
        break;
    }
  }

  // Run continuous cycle only when enabled
  if (continuousMode && !pausedMode) {
    runContinuousCycle();
  }
}

// ------------------- Helper Functions -------------------

int readIrRemote() {
  if (IrReceiver.decode()) {
    int value = IrReceiver.decodedIRData.command;
    IrReceiver.resume();
    return value;
  }
  return -1;
}

void stopAllSteppers() {
  stepperFar.setSpeed(0);
  stepperNear.setSpeed(0);
  stepperRapier.setSpeed(0);
  stepper4.setSpeed(0);

  stepperFar.runSpeed();
  stepperNear.runSpeed();
  stepperRapier.runSpeed();
  stepper4.runSpeed();
}

bool checkForStop() {
  int button = readIrRemote();

  if (button == 71) { // emergency stop
    continuousMode = false;
    pausedMode = false;
    stopAllSteppers();
    Serial.println("Emergency stop (cycle)");
    return true;
  }

  if (button == 68) { // pause
    continuousMode = false;
    pausedMode = true;
    stopAllSteppers();
    Serial.println("Paused (cycle)");
    return true;
  }

  return false;
}

void runStepperForTime(AccelStepper &stepper, int speedValue, unsigned long durationMs) {
  unsigned long startTime = millis();
  stepper.setSpeed(speedValue);

  while (millis() - startTime < durationMs) {
    stepper.runSpeed();

    // Let user interrupt during a long move
    int button = readIrRemote();
    if (button == 71) {
      continuousMode = false;
      pausedMode = false;
      stopAllSteppers();
      Serial.println("Interrupted by emergency stop");
      return;
    }
    if (button == 68) {
      continuousMode = false;
      pausedMode = true;
      stopAllSteppers();
      Serial.println("Interrupted by pause");
      return;
    }
  }
  stepper.setSpeed(0);
  stepper.runSpeed();
}

void runTwoStepperForTime(AccelStepper &stepper1, AccelStepper &stepper2, int speedValue, unsigned long durationMs) {
  unsigned long startTime = millis();
  stepper1.setSpeed(speedValue);
  stepper2.setSpeed(speedValue);

  while (millis() - startTime < durationMs) {
    stepper1.runSpeed();
    stepper2.runSpeed();

    // Let user interrupt during a long move
    int button = readIrRemote();
    if (button == 71) {
      continuousMode = false;
      pausedMode = false;
      stopAllSteppers();
      Serial.println("Interrupted by emergency stop");
      return;
    }
    if (button == 68) {
      continuousMode = false;
      pausedMode = true;
      stopAllSteppers();
      Serial.println("Interrupted by pause");
      return;
    }
  }

  stepper1.setSpeed(0);
  stepper2.setSpeed(0);
  stepper1.runSpeed();
  stepper2.runSpeed();
}

// ------------------- Rapier Motion Functions -------------------

void rapierFullOut() {
  Serial.println("Rapier full out");
  runStepperForTime(stepperRapier, RapierSpeed, RapierFull);
  rapierIsHome = false;
}

void rapierFullIn() {
  Serial.println("Rapier full in");
  runStepperForTime(stepperRapier, -RapierSpeed, RapierFull);
  rapierIsHome = false;
}

void rapierOutToHome() {
  Serial.println("Rapier out to home");
  runStepperForTime(stepperRapier, -RapierSpeed, RapierOut);
  rapierIsHome = true;
}

void rapierInToHome() {
  Serial.println("Rapier in to home");
  runStepperForTime(stepperRapier, RapierSpeed, RapierIn);
  rapierIsHome = true;
}

void rapierOut() {
  Serial.println("Rapier home to out");
  runStepperForTime(stepperRapier, RapierSpeed, RapierOut);
  rapierIsHome = false;
}

void rapierIn() {
  Serial.println("Rapier home to in");
  runStepperForTime(stepperRapier, -RapierSpeed, RapierIn);
  rapierIsHome = false;
}

// ------------------- Optional Beat-Up Functions -------------------
// These use stepperFar because code defines BeatUp constants.
// If beat-up is actually on a different motor, swap stepperFar out.

void beatUpForward() {
  Serial.println("Beat-up forward");
  runTwoStepperForTime(stepperFar, stepperNear, -BeatUpSpeed, BeatUpDist);
}

void beatUpBackward() {
  Serial.println("Beat-up backward");
  runTwoStepperForTime(stepperFar, stepperNear, BeatUpSpeed, BeatUpDist);
}

// ------------------- Home / Cycle Logic -------------------

void returnSystemHome() {
  Serial.println("Returning system to home");

  // Close all grippers / servos
  ServoRapier.write(Close);
  delay(200);
  ServoNear.write(Close);
  delay(200);
  ServoFar.write(Close);
  delay(200);

  // Bring rapier back to assumed home if needed
  rapierOutToHome();

  stopAllSteppers();
  Serial.println("System at home");
}

void runContinuousCycle() {
  Serial.println("Starting one continuous cycle");

  // Step 1: Close rapier servo to prepare/pass
  ServoRapier.write(Open);
  delay(250);

  // Step 2: Send Rapier to In
  rapierIn();
  delay(250);

  beatUpForward();
  delay(250);

  int currentPos = Close;

	for (int pos = currentPos; pos <= Open; pos++) {
	  ServoNear.write(pos);
	  ServoFar.write(pos);
	  delay(1);
	}
  delay(250);

  ServoRapier.write(Close);
  delay(250);

  beatUpBackward();
  delay(250);

  rapierOut();
  delay(250);

  ServoNear.write(Close);
  ServoFar.write(Close);
  delay(250);

  Serial.println("Continuous cycle complete");
  if (checkForStop()) return;
}