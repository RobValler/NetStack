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

#include <string>
#include <memory>

class EncryptTLS;

struct STCPIPClientParms {
    int portID{0};
    std::string localIpAddress{""};
    std::string remoteIpAddress{""};
    int maxConnectRetryAttempts{0};
};

namespace message { struct SMessage; }

class CTCPIP_Client
{
public:
    CTCPIP_Client();
    ~CTCPIP_Client() =default;

    int Start(const STCPIPClientParms& parms);
    void Stop();
    int Send(const message::SMessage& msg_data);
    int Receive(message::SMessage& msg_data);
    bool Connection();

private:
    STCPIPClientParms mConnectParms;
    int client_fd;
    bool mIsConnected{false};

    std::shared_ptr<EncryptTLS> mpTLS;
};

#endif // TCPIP_CLIENT__H
