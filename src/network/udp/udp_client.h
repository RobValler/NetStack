/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef UDP_CLIENT__H
#define UDP_CLIENT__H

#include "network_connect_parms.h"

#include "i_network_hndl.h"

#ifdef __linux__
#include <netinet/in.h>
#elif _WIN32

#else
#error "OS not supported"
#endif


namespace message {struct SMessage; }

class CUDP_Client : public INetworkHndl
{
public:
    CUDP_Client(const SConnectParms& parms);
    ~CUDP_Client();

    int Start() override;
    int Send(const message::SMessage& data) override;
    int Receive(message::SMessage& data) override;
    int Connections() override;
    void Stop() override;

private:
    SConnectParms mConnectParms;
    int mSockFD;
    sockaddr_in mServerAddr{};

};

#endif // UDP_CLIENT__H
