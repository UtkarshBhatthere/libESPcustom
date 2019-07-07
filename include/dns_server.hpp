#ifndef _DNS_SERVER_H_
#define _DNS_SERVER_H_

/* Includes */
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"

#include "mongoose.h"
#include "freertos/portmacro.h"
#include "esp_log.h"
#include "freertos/portmacro.h"

/* Important vars */
const char* dns_port = "udp://:53";
in_addr_t our_ip;

/* Method Declarations */
void dns_server(void* pvParameter);
void dns_handler(struct mg_connection *nc, int ev, void *ev_data);

/* API's */
void dns_server(void* pvParameter)
{
    struct mg_mgr dns_mgr;
    struct mg_connection *dns_con;
    mg_mgr_init(&dns_mgr, NULL);
    ESP_LOGI("DNS", "Listening on '%s'\n", dns_port);
    if ((dns_con = mg_bind(&dns_mgr, dns_port, dns_handler)) == NULL) {
      ESP_LOGE("dns_error", "Connection couldn't be establlished.");
    }
    mg_set_protocol_dns(dns_con);
    while (1){
      mg_mgr_poll(&dns_mgr, 1000);
      vTaskDelay(200/portTICK_PERIOD_MS);
    }
    mg_mgr_free(&dns_mgr);
    vTaskDelete(NULL);
}

/* Handler */
void dns_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct mg_dns_message *msg;
  struct mg_dns_resource_record *rr;
  struct mg_dns_reply reply;
  int i;

  switch (ev) {
    case MG_DNS_MESSAGE: {
      struct mbuf reply_buf;
      mbuf_init(&reply_buf, 512);
      msg = (struct mg_dns_message *) ev_data;
      reply = mg_dns_create_reply(&reply_buf, msg);

      for (i = 0; i < msg->num_questions; i++) {
        char rname[256];
        rr = &msg->questions[i];
        mg_dns_uncompress_name(msg, &rr->name, rname, sizeof(rname) - 1);
        // fprintf(stdout, "Q type %d name %s\n", rr->rtype, rname);
        if (rr->rtype == MG_DNS_A_RECORD) {
          in_addr_t addr = inet_addr("10.10.0.1");
          mg_dns_reply_record(&reply, rr, NULL, rr->rtype, 10, &addr, 4);
        }
      }
      mg_dns_send_reply(nc, &reply);
      nc->flags |= MG_F_SEND_AND_CLOSE;
      mbuf_free(&reply_buf);
      break;
    }
  }
}
#endif /* @ _DNS_SERVER_H_  */