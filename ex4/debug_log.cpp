#include "debug_log.h"
#include <cstdio>
#include <cstring>

struct DebugEvent {
    const char *format;     // must be a literal/constant (non-mutable at runtime)
    uint32_t data[3];
    TickType_t ts_ticks;    // captured at enqueue time
};

static QueueHandle_t s_queue = nullptr;

// Only the debug task will ever write to stdout/printf
static void DebugPrintTask(void *pvParameters)
{
    (void)pvParameters;

    // Single line buffer; adjust if your messages are longer.
    // We print: "[ticks] " + formatted message. Ensure room for both.
    char line[256];

    DebugEvent e;
    for (;;)
    {
        if (xQueueReceive(s_queue, &e, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        // Prefix with timestamp
        int n = std::snprintf(line, sizeof(line),
                              "[%lu] ",
                              static_cast<unsigned long>(e.ts_ticks));
        if (n < 0) n = 0;
        if (n >= static_cast<int>(sizeof(line))) n = sizeof(line) - 1;

        // Append user message (format string + three numbers)
        // IMPORTANT: format must match three integer placeholders total
        // (e.g., "%u ... %u ... %u\n"). It's okay to ignore some by using 0.
        std::snprintf(line + n, sizeof(line) - n,
                      e.format, e.data[0], e.data[1], e.data[2]);

        // Guarantee newline + NUL termination
        size_t L = strnlen(line, sizeof(line));
        if (L == 0 || line[L - 1] != '\n') {
            if (L + 1 < sizeof(line)) {
                line[L] = '\n';
                line[L + 1] = '\0';
            } else {
                // If buffer exactly full without newline, force-terminate
                line[sizeof(line) - 2] = '\n';
                line[sizeof(line) - 1] = '\0';
            }
        }

        // The ONLY use of stdout in the entire system:
        std::printf("%s", line);
        // If your platform requires it, you can fflush(stdout) here.
    }
}

bool debug_init(UBaseType_t queue_len, UBaseType_t task_stack_words)
{
    if (s_queue) return true; // already inited

    s_queue = xQueueCreate(queue_len, sizeof(DebugEvent));
    if (!s_queue) return false;

    BaseType_t ok = xTaskCreate(
        DebugPrintTask,
        "debug",
        task_stack_words,      // words (not bytes)
        nullptr,
        tskIDLE_PRIORITY + 1,  // per spec: idle + 1
        nullptr
    );

    return (ok == pdPASS);
}

void debug(const char *format, uint32_t d1, uint32_t d2, uint32_t d3)
{
    // Guard: ignore calls before init
    if (!s_queue) return;

    DebugEvent e;
    e.format   = format;
    e.data[0]  = d1;
    e.data[1]  = d2;
    e.data[2]  = d3;
    e.ts_ticks = xTaskGetTickCount();

    // Non-blocking send; drop if full to avoid stalling higher-priority tasks
    (void)xQueueSendToBack(s_queue, &e, 0);
}

void debug_from_isr(const char *format, uint32_t d1, uint32_t d2, uint32_t d3, BaseType_t *pxHigherPriorityTaskWoken)
{
    if (!s_queue) return;

    DebugEvent e;
    e.format   = format;
    e.data[0]  = d1;
    e.data[1]  = d2;
    e.data[2]  = d3;
    e.ts_ticks = xTaskGetTickCountFromISR();

    (void)xQueueSendToBackFromISR(s_queue, &e, pxHigherPriorityTaskWoken);
}
