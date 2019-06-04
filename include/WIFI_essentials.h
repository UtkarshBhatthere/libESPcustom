#ifndef _WIFI_ESSENTIALS_H
#define _WIFI_ESSENTIALS_H

/* Includes */
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_event_loop.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum{
    WIFI_STATION,
    WIFI_AP,
    WIFI_HYBRID
} wifi_Mode_t;

/* API's */
esp_err_t wifi_Init(wifi_Mode_t mode);
esp_err_t wifi_scan(void);
esp_err_t wifi_connect(void);
esp_err_t wifi_StationHandler(void *ctx, system_event_t *event);
esp_err_t wifi_APHandler(void *ctx, system_event_t *event);

/* Tasks */
void wifiSimpleTask(void* ctx);

#ifdef __cplusplus
}
#endif

#endif // @ _WIFI_ESSENTIALS_H