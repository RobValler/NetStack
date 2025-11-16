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

#include "i_network_hndl.h"

#include "network_connect_parms.h"

#include <boost/asio/ip/tcp.hpp>

#include <memory>
#include <cstdint>

struct SConnectionParms;

struct SClientConnect {
    std::string name;
    std::shared_ptr<boost::asio::ip::tcp::socket> mpSocket;
};

class CNetworkTCPIP : public INetworkHndl
{
public:
    CNetworkTCPIP(const SConnectParms& parms);
    ~CNetworkTCPIP();

    void Server() override;
    void Client() override;
    int Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) override;
    int Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) override;
    bool IsConnected() override;
    void Stop() override;

private:
    SConnectParms mConnectParms;
    std::shared_ptr<boost::asio::io_context> mpIOContext;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> mpAcceptor;
    std::shared_ptr<boost::asio::ip::tcp::resolver> mpResolver;
    std::vector<SClientConnect> mSocketList;
};

#endif // NETWORK_HNDL_TCPIP__H
