

#ifndef TCPIP_SERVER__H
#define TCPIP_SERVER__H

#include "network_connect_parms.h"

#include "i_network_hndl.h"

class CTCPIP_Server : public INetworkHndl
{
public:
    CTCPIP_Server(const SConnectParms& parms);
    ~CTCPIP_Server() =default;

    int Start() override;
    int Send(const message::SMessage& msg_data) override;
    int Receive(message::SMessage& msg_data) override;
    bool IsConnected() override;
    void Stop() override;

private:

    SConnectParms mConnectParms;
    int server_fd;
    int client_fd;
};

#endif // TCPIP_SERVER__H
