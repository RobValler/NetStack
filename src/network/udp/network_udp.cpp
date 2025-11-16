/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "network_udp.h"

#include <boost/asio.hpp>

#include "network_connect_parms.h"
#include "configuration.h"

#include <iostream>

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

    boost::asio::io_context io_context;

    try {
        while( true )
        {
            boost::asio::ip::udp::socket socket(io_context,
                                                boost::asio::ip::udp::endpoint
                                                { boost::asio::ip::udp::v4(), 3303 });

            boost::asio::ip::udp::endpoint client;
            char recv_str[1024] = {};

            socket.receive_from(boost::asio::buffer(recv_str), client);
            std::cout << client << ": " << recv_str << '\n';
        }

    } catch (std::exception& e) {
        std::cerr << "EXCEPTION HANDLED: " << e.what() << std::endl;
    }
}

void CNetwork_UDP::Client() {

    if(config::gMinPortNumber > mConnectParms.portID) {
        std::cout << "Warning: Attempted to access privilaged port number (below 1024). Check permissions!" << std::endl;
    }
    std::cout << "UDP Client started" << std::endl;

    try {

        boost::asio::io_context io_context;

        boost::asio::ip::udp::socket socket{io_context};
        socket.open(boost::asio::ip::udp::v4());

        socket.send_to(
            boost::asio::buffer("Hello world!"),
            boost::asio::ip::udp::endpoint{boost::asio::ip::make_address("192.168.35.145"), 3303});

    } catch (std::exception& e) {
        std::cerr << "EXCEPTION HANDLED: " << e.what() << std::endl;
    }
}

int CNetwork_UDP::Send() {

    int result = 0;

    return result;
}

int CNetwork_UDP::Receive() {

    int length = 0;


    return length;
}

void CNetwork_UDP::Stop() {


}
