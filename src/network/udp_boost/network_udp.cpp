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
//#include "configuration.h"

#include "message_define.h"

#include "boost/asio/ip/udp.hpp"

// #include <iostream>
// #include <thread>
// #include <chrono>

using namespace boost::asio::ip;

CNetwork_UDP::CNetwork_UDP(const SConnectParms& parms)
    : mConnectParms(parms)
{ /* nothing */ }

CNetwork_UDP::~CNetwork_UDP()
{ /* nothing */ }

int CNetwork_UDP::Send(const message::SMessage& data) {

    if(!mpSocket) {
        return 0;
    }

    int length = 0;
    length = mpSocket->send_to(boost::asio::buffer(data.data_array), *mpEndpoints.begin()); // Send the message
    return length;
}

int CNetwork_UDP::Receive(message::SMessage& data) {

    if(!mpSocket) {
        return 0;
    }

    int length = 0;
    size_t available_bytes = mpSocket->available();
    if(0 == available_bytes) {
        return 0;
    }
    data.data_array.resize(available_bytes);

    udp::endpoint sender_endpoint;
    length = mpSocket->receive_from(boost::asio::buffer(data.data_array), sender_endpoint);
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
