#include "wifiMan.hpp"
#include "ipconfig.h"
// #include "sodium/"

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
        tcpip_adapter_ip_info_t info;
        memset(&info, 0x00, sizeof(info));

        ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP)); 	/* stop AP DHCP server */
        inet_pton(AF_INET, ipconfig_DEFAULT_IP, &info.ip); /* access point is on a static IP */
        inet_pton(AF_INET, ipconfig_DEFAULT_GATEWAY, &info.gw);
        inet_pton(AF_INET, ipconfig_DEFAULT_NETMASK, &info.netmask);
        ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));
        ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP)); /* start AP DHCP server */
        ESP_LOGI(wifiManTag, "The IP is %s", ip4addr_ntoa(&info.ip));

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
            param->got_ip = true;
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
            param->sta_connected = true;
            break;
        }
        case SYSTEM_EVENT_AP_STADISCONNECTED: {
            system_event_ap_stadisconnected_t *stadisconnected = &event->event_info.sta_disconnected;
            ESP_LOGI(wifiManTag, "SYSTEM_EVENT_AP_STADISCONNECTED, mac:" MACSTR ", aid:%d", \
                    MAC2STR(stadisconnected->mac), stadisconnected->aid);
            param->sta_disconnected = true;
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

    /**********************************************************************************
     * @brief : Scan and print APs in wifi neighbourhood.
     * @param : num -> Count of available aps in the neighbourhood, returned by event
     *          scan_done -> number.
     * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
     **********************************************************************************/
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
        ESP_LOGI(wifiManTag, "Setting up Access Point mode.");
        helper::wifi_setAP(&(this->config), this->ssid, this->pass);
    }else if(this->mode == wifi_STATION_MODE){
        ESP_LOGI(wifiManTag, "Setting up Station mode.");
        helper::wifi_setStation(&(this->config), this->ssid, this->pass);
    }else if(this->mode == wifi_HYBRID_MODE){
        ESP_LOGI(wifiManTag, "Setting up Hybrid mode.");
        helper::wifi_setHybrid(&(this->config), this->ssid, this->pass);
        std::string ssid, pass;
        esp_err_t error;
        if ((error = this->fetch_config(ssid, pass)) == ESP_OK)
        {
            ESP_LOGI(wifiManTag, "Configuration Fetched, Setting up station.");
            this->ap_sta_init(ssid, pass);
        }else{
            ESP_LOGI(wifiManTag, "No configuration fetched, setting up AP only.");
        }
        return ESP_OK;
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
    esp_err_t err = esp_wifi_connect();
    return err;
}

// TODO: Test the save and feth config api's.
// TODO: Implement cryptography to encrypt password before saving.

/********************************************************************************
 * @brief : Initialise the station in hybrid mode after initialisng AP.
 * @param : 1.) ssid   -> SSID for the required Access Point.
 *          2.) pass   -> Password for required Access Point.
 * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
 ********************************************************************************/
esp_err_t wifiMan::ap_sta_init(std::string ssid, std::string pass)
{
    memset(&(this->config.sta), 0x00, sizeof(wifi_sta_config_t));
    memcpy(this->config.sta.ssid, ssid.c_str(), ssid.length());
    memcpy(this->config.sta.password, pass.c_str(), pass.length());
    this->config.sta.scan_method = WIFI_FAST_SCAN;
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &(this->config)));
    return ESP_OK;
}

/**********************************************************************************
 * @brief : Save the current Station configuration to nvs for subsequent retrieval.
 * @param : null.
 * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
 **********************************************************************************/
esp_err_t wifiMan::save_config(void)
{
    nvs_handle mem_handle;
    esp_err_t error;
    ESP_LOGI(wifiManTag, "Saving Configuration.");
    if ((error = nvs_open(wifiManNS, NVS_READWRITE, &mem_handle)) != ESP_OK){
        return error;
    }
    if ((error = nvs_set_blob(mem_handle, "ssid", this->config.sta.ssid, 32)) != ESP_OK){
        return error;
    }
    if ((error = nvs_set_blob(mem_handle, "password", this->config.sta.password, 64)) != ESP_OK){
        return error;
    }
    if ((error = nvs_commit(mem_handle))!= ESP_OK){
        return error;
    }
    nvs_close(mem_handle);
    return ESP_OK;
}

/********************************************************************************
 * @brief : Fetch the saved configuration for Station use.
 * @param : 1.) ssid   -> SSID for the required Access Point.
 *          2.) pass   -> Password for required Access Point.
 * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
 ********************************************************************************/
esp_err_t wifiMan::fetch_config(std::string &ssid_fetch, std::string &pass_fetch)
{
    nvs_handle mem_handle;
    esp_err_t error;
    ESP_LOGI(wifiManTag, "Fetching Configuration.");
    if(nvs_open(wifiManNS, NVS_READONLY, &mem_handle) == ESP_OK){
        char* ssid = new char[32];
        char* pass = new char[64];
        size_t size = sizeof(this->config.sta.ssid);
        if ((error = nvs_get_blob(mem_handle, "ssid", ssid, &size) != ESP_OK))
        {
            delete[] ssid;
            delete[] pass;
            return error;
        }
        size = sizeof(this->config.sta.password);
        if ((error = nvs_get_blob(mem_handle, "pass", pass, &size)) != ESP_OK)
        {
            delete[] ssid;
            delete[] pass;
            return error;
        }
        //transfer ssid to wifiMan and delete temporarily allocated memory.
        ssid_fetch = ssid;
        delete[] ssid;

        //transfer pass to wifiMan and delete temporarily allocated memory.
        pass_fetch = pass;
        delete[] pass;
    }
    //Done.
    return ESP_OK;
}

/**********************************************************************************
 * @brief : Check if the wifi station is connected to any AP already.
 * @param : null.
 * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
 **********************************************************************************/
bool wifiMan::isConnected(void)
{
    if(this->mode == wifi_STATION_MODE){
        return this->status.connected;
    }else{
        return false;
    }
}

/**********************************************************************************
 * @brief : Check if an IP is allocated to the wifi station by the AP dhcp server.
 * @param : null.
 * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
 **********************************************************************************/
bool wifiMan::got_ip(void)
{
    if(this->mode == wifi_STATION_MODE){
        return this->status.got_ip;
    }else{
        return false;
    }
}

/********************************************************************************
 * @brief : Iterated** manager function for wifi subsystem.
 * @param : null.
 * @note  : It has to be placed in a while(1) for repetetive check.
 * @retval: ESP_OK for sunny_day other wise ESP_ERROR_CHECK() will handle.
 ********************************************************************************/
esp_err_t wifiMan::mgr(TickType_t ticks)
{
    //TODO: Implement a good wifimgr.
    if(this->mode == wifi_STATION_MODE){
        
        if(this->status.connected == false){
            this->connect();
        }else if(this->status.connected == true){
        
        }
    }
    vTaskDelay(ticks/portTICK_PERIOD_MS);
    return ESP_OK;
}