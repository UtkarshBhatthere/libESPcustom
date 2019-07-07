#ifndef _HEAP_MONITOR_H_
#define _HEAP_MONITOR_H_

/* Includes */
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"

/* API's */

/**
 * @brief RTOS task that periodically prints the heap memory available.
 * @note Pure debug information, should not be ever started on production code! This is an example on how you can integrate your code with wifi-manager
 */
void monitoring_task(void *pvParameter)
{
	for(;;){
		ESP_LOGI("MEM_Monitor", "free heap: %d",esp_get_free_heap_size());
		vTaskDelay( pdMS_TO_TICKS(10000) );
	}
}

#endif /* @ _HEAP_MONITOR_H_ */