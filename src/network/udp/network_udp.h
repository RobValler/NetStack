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

#include "i_network_hndl.h"

#include <boost/asio.hpp>

#include <memory>
#include <atomic>

class CNetwork_UDP : public INetworkHndl
{
public:
    CNetwork_UDP(const SConnectParms& parms);
    ~CNetwork_UDP();

    void Server() override;
    void Client() override;
    int Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) override;
    int Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) override;
    bool IsConnected() override { return false; };
    void Stop() override;

private:
    SConnectParms mConnectParms;
    std::shared_ptr<boost::asio::io_context> mpIOContext;
    std::shared_ptr<boost::asio::ip::udp::socket> mpSocket;
    std::shared_ptr<boost::asio::ip::udp::resolver> mpResolver;
    boost::asio::ip::udp::resolver::results_type mpEndpoints;

    boost::asio::ip::udp::endpoint sender_endpoint;

    bool mIsInitialised{false};
    std::atomic<bool> mHasExitBeenRequested{false};


};

#endif // NETWORK_HNDL_UDP__H
