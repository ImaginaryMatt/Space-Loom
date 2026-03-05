// Setting up STM32
#include "mbed.h"
#include <sys>
#include <math.h>
#include <sys/wait.h>

// Define the pins for the 4 stepper motor controls
#define MOTOR_PWM_PIN PA_0
#define MOTOR_DIR_PIN PA_1

// Create a PWM output for motor speed control
PwmOut motor_pwm(MOTOR_PWM_PIN);
// Create a digital output for motor direction control
DigitalOut motor_dir(MOTOR_DIR_PIN);

int main() {
    // Initialize the motor control pins
    motor_pwm.period(0.001); // Set PWM period to 1 ms (1 kHz frequency)
    motor_pwm = 0.0; // Start with motor stopped
    motor_dir = 0; // Set initial direction

    while (true) {
        // Test forward direction
        motor_dir = 0; // Set direction to forward
        for (float duty_cycle = 0.0; duty_cycle <= 1.0; duty_cycle += 0.1) {
            motor_pwm = duty_cycle; // Increase speed
            wait(1); // Wait for 1 second
        }

        // Test reverse direction
        motor_dir = 1; // Set direction to reverse
        for (float duty_cycle = 0.0; duty_cycle <= 1.0; duty_cycle += 0.1) {
            motor_pwm = duty_cycle; // Increase speed
            wait(1); // Wait for 1 second
        }

        // Stop the motor before the next test cycle
        motor_pwm = 0.0; // Stop the motor
        wait(2); // Wait for 2 seconds before the next cycle
    }
}