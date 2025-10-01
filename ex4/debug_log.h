#pragma once
#include <cstdint>

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

/**
 * Initialize the debug logging system:
 *  - Creates the message queue
 *  - Spawns the debug print task (priority = idle + 1)
 *
 * Must be called before any debug(...) use.
 * Returns true on success.
 */
bool debug_init(UBaseType_t queue_len = 32, UBaseType_t task_stack_words = 512);

/**
 * Enqueue a debug message.
 * - format: MUST be a constant/literal (does not change at runtime)
 * - d1..d3: three numbers that will be formatted into 'format'
 * - A timestamp (tick count) is captured and printed at the start of the line.
 *
 * This function never uses stdout; it only enqueues.
 * If the queue is full, the message is dropped (non-blocking) to avoid priority inversion.
 */
void debug(const char *format, uint32_t d1, uint32_t d2, uint32_t d3);

/**
 * Optional ISR-safe variant. Same semantics as debug().
 */
void debug_from_isr(const char *format, uint32_t d1, uint32_t d2, uint32_t d3, BaseType_t *pxHigherPriorityTaskWoken);
