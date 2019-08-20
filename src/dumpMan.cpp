#include "dumpMan.hpp"

using namespace std;

dumpMan::dumpMan(const char* IP, int port, int protocol = SOCK_STREAM, int verbosity)
{
    this-> targetIP     = IP;
    this-> port         = port;
    this-> packet_Proto = protocol;
    this->verbosity     = verbosity;
    this-> dest_addr.sin_addr.s_addr = inet_addr(this->targetIP.c_string());
    this-> dest_addr.sin_family = AF_INET;
    this-> dest_addr.sin_port = htons(this->port);
    this-> addr_family = AF_INET;
    this-> ip_protocol = IPPROTO_IP;
    inet_ntoa_r(this-> dest_addr.sin_addr, this-> addr_str, sizeof(this->addr_str) - 1);
    // Initialising the socket.
    this->init();
}

dump_err_t dumpMan::init()
{
    this-> sock = socket(this-> addr_family, this->packet_Proto, this->ip_protocol)
    if(this-> sock < 0){
        dumpLOG(this->verbosity, "ERROR! : In creating socket.")
        return FAILURE;
    }
    dumpLOG(this->verbosity, "Socket Created Successfully!");
    return SUCCESS;
}

dump_err_t dumpMan::connect()
{
    int status = connect(this-> sock, (struct sockaddr_in*)this-> dest_addr, sizeof(dest_addr));
    if(status < 0){
        dumpLOG(this->verbosity, "ERROR! : In Connecting.")
        return FAILURE;
    }
    dumpLOG(this->verbosity, "Connected to socket Successfully!");
    return SUCCESS;
}

int dumpMan::read(char* readBuffer, int size)
{
    int len = recv(this->sock, readBuffer, size - 1, 0);
    if(size < 0){
        dumpLOG(this->verbosity, "ERROR! : Receive Failed")
        break;
    }else{
        readBuffer[len] = '\0';
        dumpLOG(this->verbosity, "Received %d bytes", len);
    }
    return len;
}

int dumpMan::write(const char* payload, int size)
{
    int status = send(this->sock, payload, size, 0);
    if(status < 0){
        dumpLOG(this->verbosity, "ERROR! : In writing data to socket.");
        return 0;
    }
    dumpLOG(this->verbosity, "Sent Data Successfully")
    return size;
}

void dumpMan::disconnect()
{
    if(sock < 0){
        dumpLOG(this->verbosity, "Socket isn't available");
        
    }else{
        dumpLOG(this->verbosity, "Disconnecting the socket.");
        shutdown(sock, 0);
        close(sock);
    }
}

// dumpMan::~dumpMan()
// {

// }