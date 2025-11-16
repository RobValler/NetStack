/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef NETWORK_HNDL_UDP__H
#define NETWORK_HNDL_UDP__H

#include "network_connect_parms.h"

#include <memory>

// namespace boost::asio { class io_context; }
// namespace boost::asio::ip::udp::v4 { class socket; }

class CNetwork_UDP
{
public:

    CNetwork_UDP(const SConnectParms& parms);
    ~CNetwork_UDP();

    void Server();
    void Client();
    int Send();
    int Receive();
    bool IsConnected() { return false; };
    void Stop();

private:
    SConnectParms mConnectParms;
    // std::shared_ptr<boost::asio::io_context> mpIOContext;
    // std::shared_ptr<boost::asio::ip::udp::v4::socket> mpSocket;

};

#endif // NETWORK_HNDL_UDP__H
