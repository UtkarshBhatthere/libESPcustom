#include "wifiMan.hpp"

//TODO: Documentation.
//Done: Implement better handler functions.

namespace helper
{
    /* Static API's */
    /********************************************************************************
     * @brief : Static function to configure the Access Point Mode.
     * @param : 1.) Config -> Pointer to wifi_config_t stored in wifi_manager.
     *          2.) ssid   -> SSID for the required Access Point.
     *          3.) pass   -> Password for required Access Point.
     * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
     ********************************************************************************/
    static esp_err_t wifi_setAP(wifi_config_t *config, std::string ssid, std::string pass)
    {
        ESP_ERROR_CHECK(esp_wifi_set_mode(wifi_AP_MODE));
        memset(config, 0x00, sizeof(wifi_config_t));
        memcpy(config->ap.ssid, ssid.c_str(), ssid.length());
        memcpy(config->ap.password, pass.c_str(), pass.length());
        config->ap.ssid_hidden = false;
        config->ap.authmode = WIFI_AUTH_WPA2_PSK;
        config->ap.channel = 0;
        config->ap.max_connection = 2;
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, config));
        return ESP_OK;
    }

    /********************************************************************************
     * @brief : Static function to configure the Station Mode.
     * @param : 1.) Config -> Pointer to wifi_config_t stored in wifi_manager.
     *          2.) ssid   -> SSID of the targeted Access Point.
     *          3.) pass   -> Password of targeted Access Point.
     * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
     ********************************************************************************/
    static esp_err_t wifi_setStation(wifi_config_t *config, std::string ssid, std::string pass)
    {
        ESP_ERROR_CHECK(esp_wifi_set_mode(wifi_STATION_MODE));
        memset(config, 0x00, sizeof(wifi_config_t));
        memcpy(config->sta.ssid, ssid.c_str(), ssid.length());
        memcpy(config->sta.password, pass.c_str(), pass.length());
        config->sta.scan_method = WIFI_FAST_SCAN;
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, config));
        return ESP_OK;
    }

    /********************************************************************************
     * @brief : Static function to configure the AP + STA.
     * @param : 1.) Config -> Pointer to wifi_config_t stored in wifi_manager.
     *          2.) ssid   -> SSID for the required Access Point.
     *          3.) pass   -> Password for required Access Point.
     * @note  : The setting of the Station is left unimplemented for later.
     * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
     ********************************************************************************/
    static esp_err_t wifi_setHybrid(wifi_config_t *config, std::string ssid, std::string pass)
    {
        ESP_ERROR_CHECK(esp_wifi_set_mode(wifi_HYBRID_MODE));
        memset(config, 0x00, sizeof(wifi_config_t));
        memcpy(config->ap.ssid, ssid.c_str() , ssid.length() );
        memcpy(config->ap.password, pass.c_str(), pass.length());
        config->ap.ssid_hidden = false;
        config->ap.authmode = WIFI_AUTH_WPA2_PSK;
        config->ap.channel = 9;
        config->ap.max_connection = 2;
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, config));
        return ESP_OK;
    }

    /**********************************************************************************
     * @brief : Event Handler for procession of various events raised during execution.
     * @param : null.
     * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
     **********************************************************************************/
    static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
    {
        wifi_status_t *param = (wifi_status_t*)ctx;
        if (event == NULL) {
            ESP_LOGE(wifiManTag, "event is null!");
            return ESP_FAIL;
        }

        switch (event->event_id) {
        case SYSTEM_EVENT_WIFI_READY: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_WIFI_READY");
            break;
        }
        case SYSTEM_EVENT_SCAN_DONE: {
            system_event_sta_scan_done_t *scan_done = &event->event_info.scan_done;
                ESP_LOGI(wifiManTag, "SYSTEM_EVENT_SCAN_DONE, status:%d, number:%d",  scan_done->status, scan_done->number);
            param->scan_done_count = scan_done->number;
            break;
        }
        case SYSTEM_EVENT_STA_START: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_START");
            break;
        }
        case SYSTEM_EVENT_STA_STOP: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_STOP");
            break;
        }
        case SYSTEM_EVENT_STA_CONNECTED: {
            system_event_sta_connected_t *connected = &event->event_info.connected;
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_CONNECTED, ssid:%s, ssid_len:%d, bssid:" MACSTR ", channel:%d, authmode:%d", \
                    connected->ssid, connected->ssid_len, MAC2STR(connected->bssid), connected->channel, connected->authmode);
            param->connected = true;
            break;
        }
        case SYSTEM_EVENT_STA_DISCONNECTED: {
            system_event_sta_disconnected_t *disconnected = &event->event_info.disconnected;
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_DISCONNECTED, ssid:%s, ssid_len:%d, bssid:" MACSTR ", reason:%d", \
                    disconnected->ssid, disconnected->ssid_len, MAC2STR(disconnected->bssid), disconnected->reason);
            param->connected = false;
            break;
        }
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: {
            system_event_sta_authmode_change_t *auth_change = &event->event_info.auth_change;
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_AUTHMODE_CHNAGE, old_mode:%d, new_mode:%d", auth_change->old_mode, auth_change->new_mode);
            break;
        }
        case SYSTEM_EVENT_STA_GOT_IP: {
            system_event_sta_got_ip_t *got_ip = &event->event_info.got_ip;
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_GOT_IP, ip:" IPSTR ", mask:" IPSTR ", gw:" IPSTR,
                IP2STR(&got_ip->ip_info.ip),
                IP2STR(&got_ip->ip_info.netmask),
                IP2STR(&got_ip->ip_info.gw));
            break;
        }
        case SYSTEM_EVENT_STA_LOST_IP: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_LOST_IP");
            break;
        }
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
            break;
        }
        case SYSTEM_EVENT_STA_WPS_ER_FAILED: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_WPS_ER_FAILED");
            break;
        }
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
            break;
        }
        case SYSTEM_EVENT_STA_WPS_ER_PIN: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_STA_WPS_ER_PIN");
            break;
        }
        case SYSTEM_EVENT_AP_START: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_AP_START");
            break;
        }
        case SYSTEM_EVENT_AP_STOP: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_AP_STOP");
            break;
        }
        case SYSTEM_EVENT_AP_STACONNECTED: {
            system_event_ap_staconnected_t *staconnected = &event->event_info.sta_connected;
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_AP_STACONNECTED, mac:" MACSTR ", aid:%d", \
                    MAC2STR(staconnected->mac), staconnected->aid);
            break;
        }
        case SYSTEM_EVENT_AP_STADISCONNECTED: {
            system_event_ap_stadisconnected_t *stadisconnected = &event->event_info.sta_disconnected;
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_AP_STADISCONNECTED, mac:" MACSTR ", aid:%d", \
                    MAC2STR(stadisconnected->mac), stadisconnected->aid);
            break;
        }
        case SYSTEM_EVENT_AP_STAIPASSIGNED: {
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_AP_STAIPASSIGNED");
            break;
        }
        case SYSTEM_EVENT_AP_PROBEREQRECVED: {
            system_event_ap_probe_req_rx_t *ap_probereqrecved = &event->event_info.ap_probereqrecved;
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_AP_PROBEREQRECVED, rssi:%d, mac:" MACSTR, \
                    ap_probereqrecved->rssi, \
                    MAC2STR(ap_probereqrecved->mac));
            break;
        }
        case SYSTEM_EVENT_GOT_IP6: {
            ip6_addr_t *addr = &event->event_info.got_ip6.ip6_info.ip;
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_AP_STA_GOT_IP6 address %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
                    IP6_ADDR_BLOCK1(addr),
                    IP6_ADDR_BLOCK2(addr),
                    IP6_ADDR_BLOCK3(addr),
                    IP6_ADDR_BLOCK4(addr),
                    IP6_ADDR_BLOCK5(addr),
                    IP6_ADDR_BLOCK6(addr),
                    IP6_ADDR_BLOCK7(addr),
                    IP6_ADDR_BLOCK8(addr));
            break;
        }
        default: {
            ESP_LOGW(wifiManTag, "unexpected system event %d!", event->event_id);
            break;
        }
        }

        return ESP_OK;
    }

    void wifi_printScan(uint16_t num)
    {
        wifi_ap_record_t *records = new wifi_ap_record_t[num];
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
                ESP_LOGI(wifiManTag, "ssid : %s, rssi: %d, authmode: %s\n", records[i].ssid, records[i].rssi, authmode); 
            }
        delete[] records;
    }
}

wifiMan::wifiMan(std::string ssid, std::string pass, wifi_mode_t mode, bool verbose)
{
    this-> ssid = ssid;
    this-> pass = pass;
    this-> mode = mode;
    this-> status.verbose = verbose;
}

/********************************************************************************
 * @brief : Wifi Subsystem Initialisation Funcion.
 * @param : mgr -> Pointer/Reference to wifi_manager_t (Wifi handler).
 * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
 ********************************************************************************/
esp_err_t wifiMan::init()
{
    // Setting UP the wifi general config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    // Setting up wifi mode config.
    if(this->mode == wifi_AP_MODE){
        helper::wifi_setAP(&(this->config), this->ssid, this->pass);
    }else if(this->mode == wifi_STATION_MODE){
        helper::wifi_setStation(&(this->config), this->ssid, this->pass);
    }else if(this->mode == wifi_HYBRID_MODE){
        helper::wifi_setHybrid(&(this->config), this->ssid, this->pass);
    }

    ESP_ERROR_CHECK(esp_event_loop_init(helper::wifi_event_handler, &(this->status)));

    //Initiating Wifi System.
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(wifiManTag, "Subsysystem Initiated");

    return ESP_OK;
}

/********************************************************************************
 * @brief : Performs a scan for nearby active Access Point in all channels.
 * @param : null.
 * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
 ********************************************************************************/
esp_err_t wifiMan::scan(void)
{
    wifi_scan_config_t scan_def = {
    .ssid = NULL,
    .bssid = NULL,
    .channel = 0,
    .show_hidden = 1
    };
    // First Parameter is scan config var next param is block_bool if true it will block the caller until the scan is done.
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_def, 1));
    return ESP_OK;
}

/********************************************************************************
 * @brief : Initiate connectioin if an access point is available.
 * @param : null.
 * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
 ********************************************************************************/
esp_err_t wifiMan::connect(void)
{
    ESP_ERROR_CHECK(esp_wifi_connect());
    return ESP_OK;
}

/********************************************************************************
 * @brief : Iterated** manager function for wifi subsystem.
 * @param : null.
 * @note  : It has to be placed in a while(1) for repetetive check.
 * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
 ********************************************************************************/
esp_err_t wifiMan::mgr(TickType_t ticks)
{
    //TODO: Check for connectedness.
    //TODO: Use scan_done_count
    //TODO: Auto Connect for lost wifi connection.
    if(this->mode == wifi_STATION_MODE){
        
        if(this->status.connected == false){
            this->connect();
        }else if(this->status.connected == true){
        
        }
    }
    vTaskDelay(ticks/portTICK_PERIOD_MS);
    return ESP_OK;
}