#if 0
#include "includes.hpp"
#include "WIFI_essentials.h"
#include "mongoose.h"

static const char*  testUrl = "ptsv2.com/t/testServer/post";
static const char* postUrl = "/t/testServer/post";
static int exit_flag = 0;

/************************************************************************************************
 * Mongoose Networking Event Handler.                                                           *
 * This is a very basic instance of the Event Handler and will be developed upon for extension. *
 **/
static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if (ev == MG_EV_HTTP_REPLY) {
    struct http_message *hm = (struct http_message *)p;
    c->flags |= MG_F_CLOSE_IMMEDIATELY;
    fwrite(hm->message.p, 1, (int)hm->message.len, stdout);
    putchar('\n');
    exit_flag = 1;
  } else if (ev == MG_EV_CLOSE) {
    exit_flag = 1;
  };
}

void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    esp_event_loop_init(wifi_StationHandler, NULL);
    wifi_Init(WIFI_STATION);
    wifi_scan();
    struct mg_mgr netMan;
    mg_mgr_init(&netMan, NULL);
    mg_connect_http(&netMan, ev_handler, testUrl, NULL, "This Data is supposed to be online now!");

    while (1){
      mg_mgr_poll(&netMan, 1000);
    }
    mg_mgr_free(&netMan);
}
#endif // Main Control.