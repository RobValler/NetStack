

#ifndef POSIX_SERVER__H
#define POSIX_SERVER__H

#include "network_connect_parms.h"

#include "i_network_hndl.h"

class CTCPIP_Server : public INetworkHndl
{
public:
    CTCPIP_Server(const SConnectParms& parms);
    ~CTCPIP_Server() =default;

    int Start() override;
    int Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) override;
    int Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) override;
    bool IsConnected() override;
    void Stop() override;

private:

    SConnectParms mConnectParms;
    int server_fd;
    int client_fd;
};

#endif // POSIX_SERVER__H
