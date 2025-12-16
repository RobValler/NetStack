/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef POSIX_MQ_NODE__H
#define POSIX_MQ_NODE__H

#include "network_connect_parms.h"

#include <mqueue.h>

#include "i_network_hndl.h"

class CPOSIX_MQ_Node : public INetworkHndl
{
public:
    CPOSIX_MQ_Node(const SConnectParms& parms);
    ~CPOSIX_MQ_Node() =default;

    int Start() override;
    int Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) override;
    int Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) override;
    bool IsConnected() override;
    void Stop() override;

private:
    SConnectParms mConnectParms;

    mqd_t mMQSend;
    mqd_t mMQRecv;
};

#endif // POSIX_MQ_NODE__H
