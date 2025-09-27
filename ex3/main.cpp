#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "uart/PicoOsUart.h"

#include <cstring>
#include <string>
#include <sstream>
#include <iostream>

#define LED 22

PicoOsUart* uart;

// Timers
TimerHandle_t inactivityTimer;
TimerHandle_t ledTimer;

// LED state
TickType_t lastLedToggleTick = 0;
int ledIntervalSec = 5;  // default

// Command buffer
static std::string cmdBuffer;

extern "C" {
    uint32_t read_runtime_ctr(void) {
        return (uint32_t)time_us_64();
    }
}

// Inactivity timer callback
void inactivityCallback(TimerHandle_t xTimer) {
    uart->send("[Inactive]\r\n");
    cmdBuffer.clear();
}

// LED timer callback
void ledTimerCallback(TimerHandle_t xTimer) {
    static bool state = false;
    state = !state;
    gpio_put(LED, state);
    lastLedToggleTick = xTaskGetTickCount();
}

// Command interpreter
void processCommand(const std::string& cmd) {
    if (cmd == "help") {
        uart->send("Commands:\r\n");
        uart->send(" help - show instructions\r\n");
        uart->send(" interval <sec> - set LED toggle interval\r\n");
        uart->send(" time - show seconds since last LED toggle\r\n");
    } else if (cmd.rfind("interval", 0) == 0) {
        std::istringstream iss(cmd);
        std::string word;
        int value;
        iss >> word >> value;
        if (value > 0) {
            ledIntervalSec = value;
            xTimerChangePeriod(ledTimer, pdMS_TO_TICKS(ledIntervalSec * 1000), 0);
            uart->send("Interval updated\r\n");
        } else {
            uart->send("Invalid interval\r\n");
        }
    } else if (cmd == "time") {
        TickType_t diff = xTaskGetTickCount() - lastLedToggleTick;
        float seconds = (diff * portTICK_PERIOD_MS) / 1000.0f;
        char buf[64];
        snprintf(buf, sizeof(buf), "%.1f sec since last toggle\r\n", seconds);
        uart->send(buf);
    } else {
        uart->send("unknown command\r\n");
    }
}

// UART input task - FIXED: Using timeout like the test code
void uartTask(void* params) {
    uint8_t buffer[64];

    // Send startup message
    uart->send("System started. Type 'help' for commands.\r\n> ");

    while (true) {
        // USE TIMEOUT instead of portMAX_DELAY - this is the key fix!
        int count = uart->read(buffer, sizeof(buffer), pdMS_TO_TICKS(30)); // 30ms timeout

        if (count > 0) {
            for (int i = 0; i < count; i++) {
                char c = buffer[i];

                // Reset inactivity timer
                xTimerReset(inactivityTimer, 0);

                // Echo character
                if (c != '\r' && c != '\n') {
                    uint8_t byte = static_cast<uint8_t>(c);
                    uart->write(&byte, 1);
                }

                if (c == '\r' || c == '\n') {
                    // Process command
                    if (!cmdBuffer.empty()) {
                        uart->send("\r\n");
                        processCommand(cmdBuffer);
                        cmdBuffer.clear();
                        uart->send("> ");
                    }
                } else if (c == '\b' || c == 127) {
                    // Handle backspace
                    if (!cmdBuffer.empty()) {
                        cmdBuffer.pop_back();
                        uint8_t backspace_seq[] = {'\b', ' ', '\b'};
                        uart->write(backspace_seq, 3);
                    }
                } else if (c >= 32 && c <= 126) {
                    // Add printable characters
                    if (cmdBuffer.length() < 100) {
                        cmdBuffer.push_back(c);
                    }
                }
            }
        }

        // Add a small delay to prevent busy waiting
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int main() {
    stdio_init_all();
    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);
    gpio_put(LED, 0);

    // Initialize UART - DON'T CHANGE PicoOsUart.cpp
    uart = new PicoOsUart(0, 0, 1, 115200);

    // Wait for UART to stabilize
    sleep_ms(100);
    uart->flush();

    // Create timers
    inactivityTimer = xTimerCreate("Inactivity", pdMS_TO_TICKS(30000), pdFALSE, 0, inactivityCallback);
    ledTimer = xTimerCreate("LedTimer", pdMS_TO_TICKS(5000), pdTRUE, 0, ledTimerCallback);

    if (inactivityTimer == NULL || ledTimer == NULL) {
        while (1) { tight_loop_contents(); }
    }

    // Start timers
    xTimerStart(ledTimer, 0);
    xTimerStart(inactivityTimer, 0);

    // Create UART task
    xTaskCreate(uartTask, "UART", 512, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1) {}
}
