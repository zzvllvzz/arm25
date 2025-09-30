#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#include <stdio.h>
#include <stdlib.h>

#define BTN1 7

//event group bits
#define EV_BIT0 (1 << 0)

extern "C" {
    uint32_t read_runtime_ctr(void) {
        return (uint32_t)time_us_64();
    }
}

//Debug event struct
struct DebugEvent {
    const char *format;
    uint32_t data[3];
    uint32_t timestamp;
};

//handle
static QueueHandle_t debugQueue;
static EventGroupHandle_t evGroup;

//debug func
void debug(const char *format, uint32_t d1, uint32_t d2, uint32_t d3) {
    DebugEvent e;
    e.format = format;
    e.data[0] = d1;
    e.data[1] = d2;
    e.data[2] = d3;
    e.timestamp = xTaskGetTickCount();

    xQueueSend(debugQueue, &e, 0);
}

// Debug print task
void debugTask(void *pv) {
    DebugEvent e;
    char buffer[128];

    while (true) {
        if (xQueueReceive(debugQueue, &e, portMAX_DELAY)) {
            snprintf(buffer, sizeof(buffer), "[%lu] ", e.timestamp);
            printf("%s", buffer);
            snprintf(buffer, sizeof(buffer), e.format,
                     e.data[0], e.data[1], e.data[2]);
            printf("%s", buffer);
        }
    }
}

// wait for button press --> set event bit
void task1(void *pv) {
    gpio_init(BTN1);
    gpio_set_dir(BTN1, GPIO_IN);
    gpio_pull_up(BTN1);

    while (true) {
        if (gpio_get(BTN1) == 0) { // pressed
            vTaskDelay(pdMS_TO_TICKS(50)); // debounce
            if (gpio_get(BTN1) == 0) {
                xEventGroupSetBits(evGroup, EV_BIT0);
                debug("Task1: Button pressed -> set bit 0\n", 0, 0, 0);

                // wait for release
                while (gpio_get(BTN1) == 0) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// wait on event bit -->then loop
void task2(void *pv) {
    // wait until bit set
    xEventGroupWaitBits(evGroup, EV_BIT0, pdFALSE, pdFALSE, portMAX_DELAY);

    TickType_t last = xTaskGetTickCount();
    while (true) {
        TickType_t now = xTaskGetTickCount();
        debug("Task2 running, elapsed %lu ticks\n", (uint32_t)(now - last), 0, 0);
        last = now;

        vTaskDelay(pdMS_TO_TICKS(1000 + (rand() % 1000))); // 1–2s
    }
}

//Wait on event bit, then loop
void task3(void *pv) {
    xEventGroupWaitBits(evGroup, EV_BIT0, pdFALSE, pdFALSE, portMAX_DELAY);

    TickType_t last = xTaskGetTickCount();
    while (true) {
        TickType_t now = xTaskGetTickCount();
        debug("Task3 running, elapsed %lu ticks\n", (uint32_t)(now - last), 0, 0);
        last = now;

        vTaskDelay(pdMS_TO_TICKS(1000 + (rand() % 1000))); // 1–2s
    }
}

int main() {
    stdio_init_all();

    //create queue and event group
    debugQueue = xQueueCreate(16, sizeof(DebugEvent));
    evGroup = xEventGroupCreate();

    //create tasks
    xTaskCreate(debugTask, "Debug", 512, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(task1, "Task1", 512, NULL, 3, NULL);
    xTaskCreate(task2, "Task2", 512, NULL, 3, NULL);
    xTaskCreate(task3, "Task3", 512, NULL, 3, NULL);

    //start scheduler
    vTaskStartScheduler();

    while (true) {}
}
