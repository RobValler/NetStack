/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef TCPIP_SERVER__H
#define TCPIP_SERVER__H

#include "network_connect_parms.h"

#include "i_network_hndl.h"

#include "tcpip_client_conn.h"

#include <thread>
#include <atomic>
#include <vector>
#include <memory>

namespace message { struct SMessage; }

class CTCPIP_Server : public INetworkHndl
{
public:
    CTCPIP_Server(const SConnectParms& parms);
    ~CTCPIP_Server() =default;

    int Start() override;
    void Stop() override;
    int Send(const message::SMessage& msg_data) override;
    int Receive(message::SMessage& msg_data) override;
    int Connections() override;

private:
    SConnectParms mConnectParms;
    int ThreadFunc();
    std::thread mtFunc;
    std::atomic<bool> mExitCaller{false};

    int mServerFD;
    std::vector<std::shared_ptr<CTCPIP_ClientConn>> mClientFDList;

};

#endif // TCPIP_SERVER__H
