#if 0
#include "includes.hpp"
#include "WIFI_essentials.h"
#include "mongoose.h"

static const char *s_address = "broker.hivemq.com:1883";
static const char *s_user_name = NULL;
static const char *s_password = NULL;
static const char *s_topic = "esp32";
static struct mg_mqtt_topic_expression s_topic_expr = {NULL, 0};

/************************************************************************************************
 * Mongoose Networking Event Handler.                                                           *
 * This is a very basic instance of the Event Handler and will be developed upon for extension. *
 **/

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
  struct mg_mqtt_message *msg = (struct mg_mqtt_message *) p;
  (void) nc;

  if (ev != MG_EV_POLL) printf("USER HANDLER GOT EVENT %d\n", ev);

  switch (ev) {
    case MG_EV_CONNECT: {
      struct mg_send_mqtt_handshake_opts opts;
      memset(&opts, 0, sizeof(opts));
      opts.user_name = s_user_name;
      opts.password = s_password;

      mg_set_protocol_mqtt(nc);
      mg_send_mqtt_handshake_opt(nc, "dummy", opts);
      break;
    }
    case MG_EV_MQTT_CONNACK:
      if (msg->connack_ret_code != MG_EV_MQTT_CONNACK_ACCEPTED) {
        printf("Got mqtt connection error: %d\n", msg->connack_ret_code);
        exit(1);
      }
      s_topic_expr.topic = s_topic;
      printf("Subscribing to '%s'\n", s_topic);
      mg_mqtt_subscribe(nc, &s_topic_expr, 1, 42);
      break;
    case MG_EV_MQTT_PUBACK:
      printf("Message publishing acknowledged (msg_id: %d)\n", msg->message_id);
      break;
    case MG_EV_MQTT_SUBACK:
      printf("Subscription acknowledged, forwarding to '/test'\n");
      break;
    case MG_EV_MQTT_PUBLISH: {
#if 0
        char hex[1024] = {0};
        mg_hexdump(nc->recv_mbuf.buf, msg->payload.len, hex, sizeof(hex));
        printf("Got incoming message %.*s:\n%s", (int)msg->topic.len, msg->topic.p, hex);
#else
      printf("Got incoming message %.*s: %.*s\n", (int) msg->topic.len,
             msg->topic.p, (int) msg->payload.len, msg->payload.p);
#endif

      printf("Forwarding to /test\n");
      mg_mqtt_publish(nc, "/test", 65, MG_MQTT_QOS(0), msg->payload.p,
                      msg->payload.len);
      break;
    }
    case MG_EV_CLOSE:
      printf("Connection closed\n");
      exit(1);
  }
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
    
    if (mg_connect(&netMan, s_address, ev_handler) == NULL) {
    fprintf(stderr, "mg_connect(%s) failed\n", s_address);
    exit(EXIT_FAILURE);
}

    while (1){
        mg_mgr_poll(&netMan, 1000);
  
    }
    mg_mgr_free(&netMan);
}
#endif // Main Control.