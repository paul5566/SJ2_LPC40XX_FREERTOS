#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

#include "board_io.h"
#include "delay.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "semphr.h"
#include "uart.h"

// GLOBAL VARIABLE
SemaphoreHandle_t FlagOfSemaphore;

void semaphore_task(char *task_name) {
  while (1) {
    if (xSemaphoreTake(FlagOfSemaphore, 1000)) {
      // Give Semaphore back:
      xSemaphoreGive(FlagOfSemaphore);
    }
  }
}

static void uart0_init(void) {
  // Note: PIN functions are initialized by board_io__initialize() for P0.2(Tx) and P0.3(Rx)
  uart__init(UART__0, clock__get_peripheral_clock_hz(), 115200);

  // Make UART more efficient by backing it with RTOS queues (optional but highly recommended with RTOS)
  QueueHandle_t tx_queue = xQueueCreate(128, sizeof(char));
  QueueHandle_t rx_queue = xQueueCreate(32, sizeof(char));
  uart__enable_queues(UART__0, tx_queue, rx_queue);
}

int main(void) {
  // UART initialization is required in order to use <stdio.h> puts, printf() etc; @see system_calls.c
  uart0_init();
  puts("\n--------\nStartup");

  FlagOfSemaphore = xSemaphoreCreateMutex();
  if (FlagOfSemaphore != NULL) {
    /* The semaphore was created successfully
     * and can be used.
     */
  }
  // printf() takes more stack space, size this tasks' stack higher
  xTaskCreate(semaphore_task, "sem_1", (512U * 8) / sizeof(void *), "task_1", PRIORITY_LOW, NULL);
  xTaskCreate(semaphore_task, "sem_2", (512U * 8) / sizeof(void *), "task_2", PRIORITY_LOW, NULL);

  puts("Starting the FREE_RTOS");
  vTaskStartScheduler();

  return 0;
}
