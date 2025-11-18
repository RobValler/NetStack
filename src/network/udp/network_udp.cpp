/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "network_udp.h"

#include "network_connect_parms.h"
#include "configuration.h"

#include "boost/asio/ip/udp.hpp"

#include <iostream>
#include <thread>
#include <chrono>

using namespace boost::asio::ip;

CNetwork_UDP::CNetwork_UDP(const SConnectParms& parms)
    : mConnectParms(parms)
{ /* nothing */ }

CNetwork_UDP::~CNetwork_UDP()
{ /* nothing */ }

void CNetwork_UDP::Server() {

    if(config::gMinPortNumber > mConnectParms.portID) {
        std::cout << "Warning: Attempted to access privilaged port number (below 1024). Check permissions!" << std::endl;
    }
    std::cout << "UDP Server started" << std::endl;

    try {

        mpIOContext = std::make_shared<boost::asio::io_context>();
        mpSocket = std::make_shared<udp::socket>(*mpIOContext, udp::endpoint(udp::v4(), 0));
        mpResolver = std::make_shared<udp::resolver>(*mpIOContext);

        mpEndpoints = mpResolver->resolve(
            udp::v4(),
            mConnectParms.ipAddress,
            std::to_string(mConnectParms.portID) );

    } catch (std::exception& e) {
        std::cerr << "EXCEPTION HANDLED: " << e.what() << std::endl;
    }

    while( !mHasExitBeenRequested ) {
        std::this_thread::sleep_for(std::chrono::seconds(1000));

    } // while
}

void CNetwork_UDP::Client() {

    if(config::gMinPortNumber > mConnectParms.portID) {
        std::cout << "Warning: Attempted to access privilaged port number (below 1024). Check permissions!" << std::endl;
    }
    std::cout << "UDP Client started" << std::endl;

    std::size_t length;
    mpIOContext = std::make_shared<boost::asio::io_context>();
    mpSocket = std::make_shared<udp::socket>(*mpIOContext, udp::endpoint(udp::v4(), mConnectParms.portID));

    while( !mHasExitBeenRequested ) {
        std::this_thread::sleep_for(std::chrono::seconds(1000));

    } // while
}

int CNetwork_UDP::Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) {

    if(!mpSocket) {
        return 0;
    }

    int length = 0;
    length = mpSocket->send_to(boost::asio::buffer(outgoing_data), *mpEndpoints.begin()); // Send the message
    return length;
}

int CNetwork_UDP::Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) {

    if(!mpSocket) {
        return 0;
    }

    int length = 0;
    size_t available_bytes = mpSocket->available();
    if(0 == available_bytes) {
        return 0;
    }
    outgoing_data.resize(available_bytes);

    udp::endpoint sender_endpoint;
    length = mpSocket->receive_from(boost::asio::buffer(outgoing_data), sender_endpoint);
    return length;
}

void CNetwork_UDP::Stop() {

    mHasExitBeenRequested = true;


    // Close the socket
    // mpSocket->shutdown(tcp::socket::shutdown_both);
    // mpSocket->close();

    // mpIOContext.reset();
    // mpSocket.reset();

}
