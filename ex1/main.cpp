#include <iostream>
#include "pico/stdlib.h"
#include <vector>
#include <queue>

// Pins configuration
const uint LED_PIN = 22;
const uint BUTTON_PINS[] = {7, 8, 9};  // Buttons for digits 0, 1, 2

// Simulated queue for button presses
std::queue<uint> buttonQueue;

// Lock states
enum LockState {
    START,
    W1,
    W2,
    W3,
    W4,
    OPEN
};

LockState currentState = START;
absolute_time_t lastButtonTime = nil_time;  // Time of last button press
absolute_time_t openLockTime = nil_time;    // Time when lock was opened

// Button debouncing states
uint lastButtonStates[3] = {1, 1, 1};  // Assume buttons are pull-up (initially high)

// Function to read buttons and simulate button reader tasks
void readButtons() {
    for (int i = 0; i < 3; i++) {
        uint buttonState = gpio_get(BUTTON_PINS[i]);

        // Detect button press (active low assumed)
        if (buttonState == 0 && lastButtonStates[i] == 1) {
            // Add button press to queue (value is the button index: 0, 1, 2)
            buttonQueue.push(i);
            lastButtonTime = get_absolute_time();
        }

        lastButtonStates[i] = buttonState;
    }
}

// Function to process button presses and simulate processing task
void processButtonPresses() {
    // Check if lock is currently open
    if (currentState == OPEN) {
        if (!is_nil_time(openLockTime) && absolute_time_diff_us(openLockTime, get_absolute_time()) > 5000000) {
            // 5 seconds have passed, close the lock
            gpio_put(LED_PIN, false);
            currentState = START;
            std::cout << "Lock closed, returning to START\n";
            openLockTime = nil_time;
        }
        return;
    }

    // Check for timeout (5 seconds without button press)
    if (!is_nil_time(lastButtonTime) && absolute_time_diff_us(lastButtonTime, get_absolute_time()) > 5000000) {
        currentState = START;
        std::cout << "Timeout, returning to START\n";
        lastButtonTime = nil_time;
        return;
    }

    // Process any button presses in the queue
    while (!buttonQueue.empty()) {
        uint receivedValue = buttonQueue.front();
        buttonQueue.pop();

        // Reset timeout timer
        lastButtonTime = get_absolute_time();

        // Process based on current state
        switch (currentState) {
            case START:
                if (receivedValue == 0) {
                    currentState = W1;
                    std::cout << "State: W1\n";
                }
                break;

            case W1:
                if (receivedValue == 0) {
                    currentState = W2;
                    std::cout << "State: W2\n";
                } else {
                    currentState = START;
                    std::cout << "Wrong digit, returning to START\n";
                }
                break;

            case W2:
                if (receivedValue == 2) {
                    currentState = W3;
                    std::cout << "State: W3\n";
                } else {
                    currentState = START;
                    std::cout << "Wrong digit, returning to START\n";
                }
                break;

            case W3:
                if (receivedValue == 1) {
                    currentState = W4;
                    std::cout << "State: W4\n";
                } else {
                    currentState = START;
                    std::cout << "Wrong digit, returning to START\n";
                }
                break;

            case W4:
                if (receivedValue == 2) {
                    currentState = OPEN;
                    std::cout << "LOCK OPENED!\n";

                    // Turn on LED and set timer
                    gpio_put(LED_PIN, true);
                    openLockTime = get_absolute_time();
                } else {
                    currentState = START;
                    std::cout << "Wrong digit, returning to START\n";
                }
                break;

            default:
                currentState = START;
                break;
        }
    }
}

int main() {
    // Initialize LED pin
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Initialize button pins
    for (int i = 0; i < 3; i++) {
        gpio_init(BUTTON_PINS[i]);
        gpio_set_dir(BUTTON_PINS[i], GPIO_IN);
        gpio_pull_up(BUTTON_PINS[i]); // Enable pull-up resistors
    }

    // Initialize serial
    stdio_init_all();
    std::cout << "\nCode Lock System Boot\n";

    // Initialize timer
    lastButtonTime = get_absolute_time();

    // Main loop
    while (true) {
        // Simulate button reader tasks
        readButtons();

        // Simulate processing task
        processButtonPresses();

        // Small delay to prevent busy waiting
        sleep_ms(10);
    }
}