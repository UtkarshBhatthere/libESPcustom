#ifndef _WIFI_ESSENTIALS_H
#define _WIFI_ESSENTIALS_H

/* Includes */
#include <string>
#include "string.h"

#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_event_legacy.h"

#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"


#define wifi_STATION_MODE   WIFI_MODE_STA
#define wifi_AP_MODE        WIFI_MODE_AP
#define wifi_HYBRID_MODE    WIFI_MODE_APSTA

#define wifiManTag  "wifiMan"

typedef struct{
    bool connected = false;
    bool sta_connected = false;
    bool sta_disconnected = false;
    bool verbose = false;
    int scan_done_count = 0;
    bool got_ip = false;
} wifi_status_t;

class wifiMan{

    private:
        std::string ssid;
        std::string pass;
        const char wifiManNS[10] = "wifiManNS";
        wifi_mode_t mode;
        wifi_config_t config;

    public:
        wifi_status_t status;

        /* API's */
        //Constructor
        wifiMan(std::string ssid, std::string pass, wifi_mode_t mode, bool verbose = false);

        // Initialise the Wifi Subsystem.
        esp_err_t init(void);

        // Scan for available wifi API's
        esp_err_t scan(void);

        // Connect to set wifi if found in current scan.
        esp_err_t connect(void);

        // Initialise the Station in apsta mode, since init only initialises ap in apsta.
        esp_err_t ap_sta_init(std::string ssid, std::string pass);

        // Save configuration of the wifi subsystem to nvs_flash.
        esp_err_t save_config(void);

        // Fetch saved config 
        esp_err_t fetch_config(std::string &ssid, std::string &pass);

        // Check connection with the wifi AP in Station mode.
        bool isConnected(void);

        // Check if IP is allotted or not.
        bool got_ip(void);

        // Wifi Manager to bind all API's.
        esp_err_t mgr(TickType_t ticks);
};

#endif // @ _WIFI_ESSENTIALS_H