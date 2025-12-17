/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef TCPIP_CLIENT__H
#define TCPIP_CLIENT__H

#include "network_connect_parms.h"

#include "i_network_hndl.h"

namespace message { struct SMessage; }

class CTCPIP_Client : public INetworkHndl
{
public:
    CTCPIP_Client(const SConnectParms& parms);
    ~CTCPIP_Client() =default;

    int Start() override;
    int Send(const message::SMessage& msg_data) override;
    int Receive(message::SMessage& msg_data) override;
    bool IsConnected() override;
    void Stop() override;

private:
    SConnectParms mConnectParms;
    int sock;
    bool mIsConnected{false};
};

#endif // TCPIP_CLIENT__H
