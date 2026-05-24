/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef UDP_STACK__H
#define UDP_STACK__H

#include "i_network_hndl.h"

#ifdef __linux__
#include <netinet/in.h>
#elif _WIN32
//
#endif

#include <string>


struct SUDPParms {

    int portLocalID{0};
    int portRemoteID{0};
    bool broadcaster{false};
    std::string broadcastIpAddress{"0.0.0.0"};
};

namespace message {struct SMessage; }

class CUDP_Stack
{
public:
    CUDP_Stack() =default;
    ~CUDP_Stack() =default;

    int Start(const SUDPParms& parms);
    void Stop();
    int Send(const message::SMessage& data);
    int Receive(message::SMessage& data);

private:
    SUDPParms mConnectParms;
    sockaddr_in mRemoteAddr{};
    sockaddr_in mBroadcastAddr{};

#ifdef __linux__
    int mSocket;
#elif _WIN32
    SOCKET mLocalSockFD;
#endif

};

#endif // UDP_STACK__H
