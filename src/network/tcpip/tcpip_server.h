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

#include "tcpip_server_conn.h"

#include <thread>
#include <atomic>
#include <vector>
#include <memory>

struct STCPIPServParms {
    int portID{0};
};

namespace message { struct SMessage; }

class CTCPIP_Server
{
public:
    CTCPIP_Server() =default;
    ~CTCPIP_Server() =default;

    int Start(const STCPIPServParms& parms);
    void Stop();
    int Send(const message::SMessage& msg_data);
    int Receive(message::SMessage& msg_data);
    int Connections();

private:
    STCPIPServParms mConnectParms;

    int ThreadFunc();
    std::thread mtFunc;
    std::atomic<bool> mExitCaller{false};

    int mServerFD;
    std::vector<std::shared_ptr<CTCPIP_ClientConn>> mClientFDList;

};

#endif // TCPIP_SERVER__H
