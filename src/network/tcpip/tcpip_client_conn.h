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
    int ID{0};
    int client_fd{0};
    std::string name{""};
    std::string ipaddress{""};
};

namespace message { struct SMessage; }

class CTCPIP_ClientConn
{
public:
    CTCPIP_ClientConn(SClientEntryCont parms)
        : mID(parms.ID)
        , mClientFD(parms.client_fd)
        , mName(parms.name)
        , mIPAaddress(parms.ipaddress)
    {}
    ~CTCPIP_ClientConn() =default;

    int GetID() {
        return mID;
    }
    int Receive(message::SMessage& msg_data);
    int Send(const message::SMessage& msg_data);
    void Stop();
    bool Connected();

private:

    int mID;
    int mClientFD;
    std::string mName;
    std::string mIPAaddress;

    bool mExitCaller{false};
    bool mConnected{true}; // class instance exists after connection
};

#endif // TCPIP_CLIENT_CON__H
