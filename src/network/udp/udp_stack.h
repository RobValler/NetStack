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

#include "network_connect_parms.h"

#include "i_network_hndl.h"

#ifdef __linux__
#include <netinet/in.h>
#elif _WIN32
//
#endif

namespace message {struct SMessage; }

class CUDP_Stack : public INetworkHndl
{
public:
    CUDP_Stack(const SConnectParms& parms);
    ~CUDP_Stack() =default;

    int Start() override;
    int Send(const message::SMessage& data) override;
    int Receive(message::SMessage& data) override;
    int Connections() override;
    void Stop() override;

private:
    SConnectParms mConnectParms;
    sockaddr_in mLocalAddr{};
    sockaddr_in mRemoteAddr{};

#ifdef __linux__
    int mLocalSockFD;
#elif _WIN32
    SOCKET mLocalSockFD;
#endif

};

#endif // UDP_STACK__H
