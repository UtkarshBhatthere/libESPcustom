#ifndef _WIFI_ESSENTIALS_H
#define _WIFI_ESSENTIALS_H

/* Includes */
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_event_loop.h"

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

/* Static API's */
static esp_err_t wifi_SetupStation(void)
{
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifi_config_t station = {
        .sta = {
        .ssid = "Test",
        .password = "qwertyuiop",
        .bssid_set = 0
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &station));
    return ESP_OK;
}

static esp_err_t wifi_SetupAP(void)
{
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    wifi_config_t ap = {
        .ap = {
        .ssid = "Test AccessPoint",
        .password = "qwertyuiop",
        .ssid_hidden = false,
        .authmode = WIFI_AUTH_WPA2_PSK,
        .channel = 9,
        .max_connection = 2
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap));
    return ESP_OK;
}

void wifi_printScan(uint16_t num){
    wifi_ap_record_t *records = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t)*num);
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&num, records));

        for(int i = 0; i<num; i++){
            char *authmode;

            switch(records[i].authmode) {
                case WIFI_AUTH_OPEN:
                authmode = "WIFI_AUTH_OPEN";
                break;
                case WIFI_AUTH_WEP:
                authmode = "WIFI_AUTH_WEP";
                break;
                case WIFI_AUTH_WPA_PSK:
                authmode = "WIFI_AUTH_WPA_PSK";
                break;
                case WIFI_AUTH_WPA2_PSK:
                authmode = "WIFI_AUTH_WPA2_PSK";
                break;
                case WIFI_AUTH_WPA_WPA2_PSK:
                authmode = "WIFI_AUTH_WPA_WPA2_PSK";
                break;
                default:
                authmode = "Unknown";
                break;
            }
            printf(" WIFI_NAME : %s, WIFI_RSSI: %d, WIFI_AUTHMODE: %s\n", records[i].ssid, records[i].rssi, authmode); 
        }
    free(records);
}

/* API's */
esp_err_t wifi_Init(wifi_Mode_t mode)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    if (mode == WIFI_STATION){
        ESP_ERROR_CHECK(wifi_SetupStation());
    }else{
        ESP_ERROR_CHECK(wifi_SetupAP());
    }
    ESP_ERROR_CHECK(esp_wifi_start());
    printf("WiFi initiated successfully.\n");
    return ESP_OK;
}

esp_err_t wifi_scan(void)
{
    wifi_scan_config_t def = {
    .ssid = NULL,
    .bssid = NULL,
    .channel = 0,
    .show_hidden = 1
    };
    // First Parameter is scan config var next param is block_bool if true it will block the caller until the scan is done.
    ESP_ERROR_CHECK(esp_wifi_scan_start(&def, 1));
    return ESP_OK;
}

esp_err_t wifi_connect(void)
{
    ESP_ERROR_CHECK(esp_wifi_connect());
    return ESP_OK;
}

esp_err_t wifi_StationHandler(void *ctx, system_event_t *event)
{
    if(event->event_id == SYSTEM_EVENT_SCAN_DONE){
        if(event->event_info.scan_done.number){
            printf("%d Access Point(s) available.", event->event_info.scan_done.number);
            wifi_printScan(event->event_info.scan_done.number);
            ESP_ERROR_CHECK(esp_wifi_connect());
        }else
            printf("Sorry, No Access Point available.");
    }
    if((event->event_id == SYSTEM_EVENT_STA_CONNECTED)){
        printf("System Connected to given Access Point.");
    }
    if(event->event_id == SYSTEM_EVENT_STA_GOT_IP){
        printf("Alotted IP address" IPSTR "\n",IP2STR(&event->event_info.got_ip.ip_info.ip));
    }
    if(event->event_id == SYSTEM_EVENT_STA_DISCONNECTED ){
        printf("System Disconnected from Access Point");
        ESP_ERROR_CHECK(esp_wifi_disconnect());
    }
    return ESP_OK;
}

esp_err_t wifi_APHandler(void *ctx, system_event_t *event)
{
    if(event->event_id == SYSTEM_EVENT_AP_STACONNECTED){
        wifi_sta_list_t records;
        esp_wifi_ap_get_sta_list(&records);
        uint8_t num = records.num;
        printf("A station connected: Total %d\n", num);
        // for(int k=0;k<=num;k++){
        // printf("Device %d ip :" IPSTR "\n", k, IP2STR(&(records->ip)));
        // }
    }
    return ESP_OK;
}

void wifiSimpleTask(void* ctx){
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_StationHandler, NULL));
    ESP_ERROR_CHECK(wifi_Init(WIFI_STATION));
    ESP_ERROR_CHECK(wifi_scan());
    while(1){
        wifi_connect();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
#endif // @ _WIFI_ESSENTIALS_H