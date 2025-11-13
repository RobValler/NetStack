/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef NETWORK_HNDL_TCPIP__H
#define NETWORK_HNDL_TCPIP__H

#include <boost/asio/ip/tcp.hpp>

#include "network_connect_parms.h"

#include <atomic>
#include <memory>

struct SConnectionParms;

class CNetworkTCPIP
{
public:
    CNetworkTCPIP(const SConnectParms& parms);
    ~CNetworkTCPIP();

    void Server();
    void Client();
    int Send(const std::vector<char>& outgoing_data);
    int Receive(std::vector<char>& outgoing_data);
    bool IsConnected();
    void Stop();

private:
    SConnectParms mConnectParms;
    std::atomic<int> mNumberOfConnections;
    std::atomic<bool> mEnoughConnections{false};

    //
    std::shared_ptr<boost::asio::io_context> mpIOContext;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> mpAcceptor;
    std::shared_ptr<boost::asio::ip::tcp::socket> mpSocket;
    std::shared_ptr<boost::asio::ip::tcp::resolver> mpResolver;
};

#endif // NETWORK_HNDL_TCPIP__H
