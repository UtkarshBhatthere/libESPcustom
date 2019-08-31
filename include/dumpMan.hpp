#ifndef _DUMP_MANAGER_HPP_
#define _DUMP_MANAGER_HPP_

#include <string>

#include "esp_err.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

using namespace std;

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
        string targetIP;
        char addr_str[128];
        int addr_family;
        int ip_protocol;
        int packet_Proto;
        struct sockaddr_in dest_addr;
        
    public:
        dumpMan(const char* IP, int port,  int protocol = SOCK_STREAM, int verbosity = 0);
        // ~dumpMan(void);
        dump_err_t init(void);
        dump_err_t get_connected(void);
        int write(const uint16_t* payload, uint16_t size);
        int read(char* readBuffer, int size);
        void get_disconnected(void);

};

#endif /*  _DUMP_MANAGER_HPP_  */