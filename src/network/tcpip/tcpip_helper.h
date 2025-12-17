/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef TCPIP_HELPER__H
#define TCPIP_HELPER__H


#include "i_network_hndl.h"

class CTCPIP_Helper : public INetworkHndl
{
public:
    CTCPIP_Helper() =default;
    ~CTCPIP_Helper() =default;

    int Send(const message::SMessage& msg_data) override;
    int Receive(message::SMessage& msg_data) override;
    bool IsConnected() override;

protected:
    int server_fd;
    int client_fd;
};

#endif // TCPIP_HELPER__H
