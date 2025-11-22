/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef NETWORK_POSIX__H
#define NETWORK_POSIX__H

#include "i_network_hndl.h"

#include <mqueue.h>

#include "network_connect_parms.h"

#include <string>

class CNetwork_POSIX : public INetworkHndl
{
public:
    CNetwork_POSIX(const SConnectParms& parms);
    ~CNetwork_POSIX();

    bool Server() override;
    bool Client() override;
    int Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) override;
    int Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) override;
    bool IsConnected() override;
    void Stop() override;

private:
    SConnectParms mConnectParms;
    void ThreadFuncServer();

    std::string mPOSIXName;
    mqd_t mPOSIXChannel;
    bool mIsConnected{false};
    bool mExitCalled{false};

};

#endif // NETWORK_POSIX__H
