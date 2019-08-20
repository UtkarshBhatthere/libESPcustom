#ifndef _DUMP_MANAGER_HPP_
#define _DUMP_MANAGER_HPP_

#include "esp_err.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#define dumpLOG(x, y) if(x) { ESP_LOGI("dump_LOG", y); }

typedef enum
{
    FAILURE = 0,
    SUCCESS = !FAILURE
} dump_err_t;

class dumpMan
{
    private:
        int verbosity;
        int sock;
        uint16_t port;
        std::string targetIP;
        char addr_str[128];
        int addr_family;
        int ip_protocol;
        int packet_Proto;
        struct sockaddr_in dest_addr;
        
    public:
        dumpMan(const char* IP, int port,  int protocol = SOCK_STREAM, int verbosity = 0);
        // ~dumpMan(void);
        dump_err_t init(void);
        dump_err_t connect(void);
        int write(const char* payload, int size);
        int read(char* readBuffer, int size);
        void disconnect(void)

};

#endif /*  _DUMP_MANAGER_HPP_  */