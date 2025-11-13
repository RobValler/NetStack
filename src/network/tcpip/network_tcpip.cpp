/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "network_tcpip.h"

#include <boost/asio.hpp>

#include "network_connect_parms.h"

#include <iostream>

namespace {
    constexpr int gReceiveBufferSize = 2048;
}

using namespace boost::asio::ip;

CNetworkTCPIP::CNetworkTCPIP(const SConnectParms& parms)
    : mConnectParms(parms)
{ /* nothing */ }

CNetworkTCPIP::~CNetworkTCPIP()
{ /* nothing */ }

void CNetworkTCPIP::Server() {

    if(1024 > mConnectParms.portID) {
        std::cout << "Warning: Attempted to access privilaged port number (below 1024). Check permissions!" << std::endl;
    }
    std::cout << "Server started" << std::endl;

    try {
        mpIOContext = std::make_shared<boost::asio::io_context>();
        mpAcceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*mpIOContext, tcp::endpoint(tcp::v4(), 12345));
        mpSocket = std::make_shared<boost::asio::ip::tcp::socket>(*mpIOContext);

        // Start accepting connections
        while (true) {
            mpAcceptor->accept(*mpSocket);
            std::cout << "Accepted a connection!" << std::endl;
            mEnoughConnections = true;
        }
    } catch (std::exception& e) {
        std::cerr << "EXCEPTION HANDLED: " << e.what() << std::endl;
    }
}

void CNetworkTCPIP::Client() {

    if(1024 > mConnectParms.portID) {
        std::cout << "Warning: Attempted to access privilaged port number (below 1024). Check permissions!" << std::endl;
    }
    std::cout << "Client started" << std::endl;

    try {
        mpIOContext = std::make_shared<boost::asio::io_context>();
        mpAcceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*mpIOContext);
        mpSocket = std::make_shared<boost::asio::ip::tcp::socket>(*mpIOContext);
        mpResolver = std::make_shared<boost::asio::ip::tcp::resolver>(*mpIOContext);

        boost::asio::connect(*mpSocket, mpResolver->resolve("127.0.0.1", "12345"));
        std::cout << "Connected to server!" << std::endl;

        mEnoughConnections = true;

    } catch (std::exception& e) {
        std::cerr << "EXCEPTION HANDLED: " << e.what() << std::endl;
    }
}

int CNetworkTCPIP::Send(const std::vector<char>& outgoing_data) {

    int result = 0;
    if(mEnoughConnections) {
        if(outgoing_data.size() > 0) {
            result = boost::asio::write(*mpSocket, boost::asio::buffer(outgoing_data));
        } else {
            std::cout << "Write cannot be performed without at least one connection" << std::endl;
        }
    }
    return result;
}

int CNetworkTCPIP::Receive(std::vector<char>& outgoing_data) {

    size_t length = 0;

    if(mEnoughConnections) {

        // check the size of the next message
        size_t available_bytes = mpSocket->available();
        if( 0 == available_bytes) {
            return 0;
        }

        // read
        std::vector<char> readBuffer(available_bytes);
        boost::system::error_code error;
        length = mpSocket->read_some(boost::asio::buffer(readBuffer), error);

        // check for errors
        if (error) {
            mEnoughConnections = false;
            std::cerr << "ERROR: " << error.message() << std::endl;
        } else {
            if(length == available_bytes) {

                outgoing_data = readBuffer;
            }
        }
    }

    return static_cast<int>(length);
}

bool CNetworkTCPIP::IsConnected() {

    return mEnoughConnections;
}

void CNetworkTCPIP::Stop() {

    mpIOContext.reset();
    mpAcceptor.reset();
    mpSocket.reset();
    mpResolver.reset();

    // Close the socket
    mEnoughConnections = false;
    mpSocket->shutdown(tcp::socket::shutdown_both);
    mpSocket->close();
}
