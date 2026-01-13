/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef TCPIP_CLIENT_CON__H
#define TCPIP_CLIENT_CON__H

#include <string>

struct SClientEntryCont {
    int mConnectionID{0};
#ifdef __linux__
    int mClientFD{0};
#elif _WIN32
    SOCKET mClientFD{0};
#endif
    std::string mIPAaddress{""};
};

namespace message { struct SMessage; }

class CTCPIP_ClientConn
{
public:
    CTCPIP_ClientConn(SClientEntryCont parms)
        : mParms(parms)
    {}
    ~CTCPIP_ClientConn() =default;

    int Receive(message::SMessage& msg_data);
    int Send(const message::SMessage& msg_data);
    void Stop();
    bool Connected();
    int GetConnectionID();

private:
    SClientEntryCont mParms;

    bool mExitCaller{false};
    bool mConnected{true}; // class instance exists after connection
};

#endif // TCPIP_CLIENT_CON__H
