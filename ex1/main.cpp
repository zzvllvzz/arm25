#include <iostream>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Pins configuration
const uint LED_PIN = 22;
const uint BUTTON_PINS[] = {7, 8, 9};  // Buttons for digits 0, 1, 2

// Queue and task handles
QueueHandle_t buttonQueue;
TaskHandle_t processingTaskHandle;

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

// Required for FreeRTOS on RP2040
extern "C" {
    uint32_t read_runtime_ctr(void) {
        return (uint32_t)time_us_64();
    }
}

// Button reader task
void buttonReaderTask(void* params) {
    uint buttonPin = *(uint*)params;
    uint lastButtonState = 1;  // Assume pull-up, initially high

    while (true) {
        uint buttonState = gpio_get(buttonPin);

        // Detect button press (active low assumed)
        if (buttonState == 0 && lastButtonState == 1) {
            // Send button press to queue
            uint buttonValue = buttonPin - BUTTON_PINS[0]; // Convert pin to value (0, 1, 2)
            xQueueSend(buttonQueue, &buttonValue, 0);
        }

        lastButtonState = buttonState;
        vTaskDelay(pdMS_TO_TICKS(20)); // Debounce delay
    }
}

// Processing task
void processingTask(void* params) {
    uint receivedValue;
    bool timeoutReported = false;

    while (true) {
        // Wait for button press with 5 second timeout
        if (xQueueReceive(buttonQueue, &receivedValue, pdMS_TO_TICKS(5000)) == pdTRUE) {
            // Reset timeout flag since we received a button press
            timeoutReported = false;

            // Process based on current state
            switch (currentState) {
                case START:
                    if (receivedValue == 0) {
                        currentState = W1;
                        std::cout << "State: W1\n";
                    } else {
                        std::cout << "Wrong digit, staying in START\n";
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

                        // Turn on LED for 5 seconds
                        gpio_put(LED_PIN, true);
                        vTaskDelay(pdMS_TO_TICKS(5000));
                        gpio_put(LED_PIN, false);

                        currentState = START;
                        std::cout << "Returning to START\n";
                    } else {
                        currentState = START;
                        std::cout << "Wrong digit, returning to START\n";
                    }
                    break;

                default:
                    currentState = START;
                    break;
            }
        } else {
            // Timeout occurred
            if (!timeoutReported) {
                // Only report timeout once
                currentState = START;
                std::cout << "Timeout, returning to START\n";
                timeoutReported = true;
            }
            // Else: Already reported timeout, just stay in this state quietly
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

    // Wait for serial connection
    sleep_ms(2000);
    std::cout << "\nCode Lock System Boot with FreeRTOS\n";

    // Create button queue
    buttonQueue = xQueueCreate(10, sizeof(uint));

    // Create button reader tasks
    for (int i = 0; i < 3; i++) {
        uint* pin = new uint(BUTTON_PINS[i]);
        xTaskCreate(buttonReaderTask,
                   "ButtonReader",
                   512,  // Stack size (2KB as requested)
                   pin,
                   1,
                   NULL);
    }

    // Create processing task
    xTaskCreate(processingTask,
               "Processing",
               512,  // Stack size (2KB as requested)
               NULL,
               2,
               &processingTaskHandle);

    // Start scheduler
    vTaskStartScheduler();

    // Should never reach here
    while (true) {
        // Emergency blink if scheduler fails
        gpio_put(LED_PIN, true);
        sleep_ms(100);
        gpio_put(LED_PIN, false);
        sleep_ms(100);
    }
}
