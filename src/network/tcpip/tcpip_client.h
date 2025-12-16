


#ifndef TCPIP_CLIENT__H
#define TCPIP_CLIENT__H

#include "network_connect_parms.h"

#include "i_network_hndl.h"

class CTCPIP_Client : public INetworkHndl
{
public:
    CTCPIP_Client(const SConnectParms& parms);
    ~CTCPIP_Client() =default;

    int Start() override;
    int Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) override;
    int Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) override;
    bool IsConnected() override;
    void Stop() override;

private:
    SConnectParms mConnectParms;
    int sock;
    bool mIsConnected{false};
};

#endif // TCPIP_CLIENT__H
