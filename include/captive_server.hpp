#ifndef _CAPTIVE_SERVER_H_
#define _CAPTIVE_SERVER_H_

/* Includes */
#include "index.h"
#include "wifiMan.hpp"

#include "mongoose.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#include <string>

using namespace std;

//Vars
string ssid;
string pass;

// Status Vars.
bool settings_arrived = false;
bool wifi_has_ip = false;
bool wifi_connected_to_ap = false;

/* Method Declarations */
void web_server(void* pvParameter);
static void captive_handler(struct mg_connection *nc, int ev, void *ev_data);
void wifi_service(void *pvParameter);

/* API's */
void web_server(void* pvParameter)
{
  struct mg_mgr captive_mgr;
  struct mg_connection *captive_con;

  mg_mgr_init(&captive_mgr, NULL);

  captive_con = mg_bind(&captive_mgr, "80", captive_handler);
  mg_set_protocol_http_websocket(captive_con);
  while (1)
  {
    mg_mgr_poll(&captive_mgr, 1000);
    vTaskDelay(200/portTICK_PERIOD_MS);
    // ESP_LOGI("config_service", "%d", uxTaskGetStackHighWaterMark(NULL));
  }
  mg_mgr_free(&captive_mgr);
}

void wifi_service(void *pvParameter)
{
    wifiMan wMgr("Test", "qwertymnb123", wifi_HYBRID_MODE);
    wMgr.init();
    while (1){
        if(settings_arrived){
            // ESP_LOGI("CHECK", "GOING TO START STA %s, %s", ssid.c_str(), pass.c_str());
            ESP_ERROR_CHECK(wMgr.ap_sta_init(ssid, pass));
            settings_arrived = 0;
            if((wMgr.connect()) == ESP_OK){
                wMgr.save_config();
            }
            vTaskDelay(1000);
        }
        wifi_has_ip = wMgr.got_ip();
        wifi_connected_to_ap = wMgr.isConnected();
        wMgr.mgr(1000);
        taskYIELD();
    }
}

/* Handler */
static void captive_handler(struct mg_connection *nc, int ev, void *ev_data) {
 struct http_message *hm = (struct http_message *) ev_data;
 
 switch (ev) {
    case MG_EV_ACCEPT: {
      char addr[32];
      mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
      printf("%p: Connection from %s\r\n", nc, addr);
      break;
    }
    case MG_EV_HTTP_REQUEST: {
      if(hm->method.p[0] == 'P'){
        ESP_LOGI("DEBUG TAG", "Received Post Request");
        char* ssid_bfr = new char[50];
        char* pass_bfr = new char[50];
        mg_get_http_var(&(hm->body), "ssid", ssid_bfr, hm->body.len);
        mg_get_http_var(&(hm->body), "password", pass_bfr, hm->body.len);
        ssid = ssid_bfr;
        pass = pass_bfr;
        settings_arrived = true;
        delete[] ssid_bfr;
        delete[] pass_bfr;
      }else if(hm->method.p[0] == 'G'){
          struct http_message *hm = (struct http_message *) ev_data;
          printf("%p: The request was --%.*s--\r\n", nc, (int) hm->method.len, hm->method.p);
          mg_send_response_line(nc, 200,"Content-Type: text/html\r\n""Connection: close");
          mg_printf(nc,"\r\n%s\r\n",page);
          nc->flags |= MG_F_SEND_AND_CLOSE;
      }
      break;
    }
    case MG_EV_CLOSE: {
      printf("%p: Connection closed\r4\n", nc);
      break;
    }
  }
}

#endif /* @ _CAPTIVE_SERVER_H_ */