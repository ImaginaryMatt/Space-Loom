#include <Arduino.h>
#include <Servo.h>
#include <AccelStepper.h>
#include <IRremote.hpp>
#include <Bounce2.h>

Bounce2::Button button = Bounce2::Button(); // INSTANTIATE A Bounce2::Button OBJECT

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
const int BeatUpDist = 2000;

const int RapierSpeed = 500;
const int RapierOut = 1600;
const int RapierIn = 1900;
const int RapierFull = 3400;

// Unsure which stepper is where
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

// ---- Control flags ----
bool continuousMode = false;
bool pausedMode = false;

// ---- Optional tracking for home state ----
bool rapierIsHome = true;

// Function declarations
int readIrRemote();
void stopAllSteppers();
void runStepperForTime(AccelStepper &stepper, int speedValue, unsigned long durationMs);
void rapierFullOut();
void rapierFullIn();
void rapierOutToHome();
void rapierInToHome();
void rapierHomeToOut();
void rapierHomeToIn();
void beatUpForward();
void beatUpBackward();
void returnSystemHome();
void runContinuousCycle();

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

  // Start at a known resting position
  ServoRapier.write(Close);
  ServoClose.write(Close);
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
        ServoClose.write(Open);
        break;
      }
      case 82:{
        // Close Close Servo
        ServoClose.write(Close);
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
        rapierHomeToOut();
        break;
      }
      case 94: {
        // Rapier Home to In
        rapierHomeToIn();
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
  stepper1.setSpeed(0);
  stepper2.setSpeed(0);
  stepper3.setSpeed(0);
  stepper4.setSpeed(0);

  stepper1.runSpeed();
  stepper2.runSpeed();
  stepper3.runSpeed();
  stepper4.runSpeed();
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

// ------------------- Rapier Motion Functions -------------------

void rapierFullOut() {
  Serial.println("Rapier full out");
  runStepperForTime(stepper3, RapierSpeed, RapierFull);
  rapierIsHome = false;
}

void rapierFullIn() {
  Serial.println("Rapier full in");
  runStepperForTime(stepper3, -RapierSpeed, RapierFull);
  rapierIsHome = false;
}

void rapierOutToHome() {
  Serial.println("Rapier out to home");
  runStepperForTime(stepper3, -RapierSpeed, RapierOut);
  rapierIsHome = true;
}

void rapierInToHome() {
  Serial.println("Rapier in to home");
  runStepperForTime(stepper3, RapierSpeed, RapierIn);
  rapierIsHome = true;
}

void rapierHomeToOut() {
  Serial.println("Rapier home to out");
  runStepperForTime(stepper3, RapierSpeed, RapierOut);
  rapierIsHome = false;
}

void rapierHomeToIn() {
  Serial.println("Rapier home to in");
  runStepperForTime(stepper3, -RapierSpeed, RapierIn);
  rapierIsHome = false;
}

// ------------------- Optional Beat-Up Functions -------------------
// These use stepper1 because code defines BeatUp constants.
// If beat-up is actually on a different motor, swap stepper1 out.

void beatUpForward() {
  Serial.println("Beat-up forward");
  runStepperForTime(stepper1, BeatUpSpeed, BeatUpDist);
}

void beatUpBackward() {
  Serial.println("Beat-up backward");
  runStepperForTime(stepper1, -BeatUpSpeed, BeatUpDist);
}

// ------------------- Home / Cycle Logic -------------------

void returnSystemHome() {
  Serial.println("Returning system to home");

  // Close all grippers / servos
  ServoRapier.write(Close);
  delay(200);
  ServoClose.write(Close);
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

  // Step 1: Open rapier servo to prepare/pass
  ServoRapier.write(Open);
  delay(250);

  // Step 2: Open close-side clamp
  ServoClose.write(Open);
  delay(250);

  // Step 3: Send rapier out
  rapierHomeToOut();
  if (!continuousMode) return;

  // Step 4: Close close-side clamp
  ServoClose.write(Close);
  delay(250);

  // Step 5: Open far-side clamp
  ServoFar.write(Open);
  delay(250);

  // Step 6: Bring rapier back home
  rapierOutToHome();
  if (!continuousMode) return;

  // Step 7: Close far-side clamp
  ServoFar.write(Close);
  delay(250);

  // Step 8: Close rapier servo
  ServoRapier.write(Close);
  delay(250);

  // Step 9: Optional beat-up motion
  beatUpForward();
  if (!continuousMode) return;

  beatUpBackward();
  if (!continuousMode) return;

  Serial.println("Continuous cycle complete");
}
