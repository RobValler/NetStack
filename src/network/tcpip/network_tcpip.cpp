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
#include "configuration.h"

#include <iostream>

using namespace boost::asio::ip;

CNetworkTCPIP::CNetworkTCPIP(const SConnectParms& parms)
    : mConnectParms(parms)
{ /* nothing */ }

CNetworkTCPIP::~CNetworkTCPIP()
{ /* nothing */ }

void CNetworkTCPIP::Server() {

    if(config::gMinPortNumber > mConnectParms.portID) {
        std::cout << "Warning: Attempted to access privilaged port number (below 1024). Check permissions!" << std::endl;
    }
    std::cout << "Server started" << std::endl;

    try {
        mpIOContext = std::make_shared<boost::asio::io_context>();
        mpAcceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*mpIOContext, tcp::endpoint(tcp::v4(), mConnectParms.portID));


        // Start accepting connections
        while (true) {

            SClientConnect local_client_connection;
            local_client_connection.name = mConnectParms.name;
            local_client_connection.mpSocket = std::make_shared<boost::asio::ip::tcp::socket>(*mpIOContext);

            mpAcceptor->accept(*local_client_connection.mpSocket);

            mSocketList.emplace_back(std::move(local_client_connection));

            std::cout << "Accepted a connection!" << std::endl;
            mEnoughConnections = true;
        }
    } catch (std::exception& e) {
        std::cerr << "EXCEPTION HANDLED: " << e.what() << std::endl;
    }
}

void CNetworkTCPIP::Client() {

    if(config::gMinPortNumber > mConnectParms.portID) {
        std::cout << "Warning: Attempted to access privilaged port number (below 1024). Check permissions!" << std::endl;
    }
    std::cout << "Client started" << std::endl;

    try {
        mpIOContext = std::make_shared<boost::asio::io_context>();
        mpAcceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*mpIOContext);
//        mpSocket = std::make_shared<boost::asio::ip::tcp::socket>(*mpIOContext);
        mpResolver = std::make_shared<boost::asio::ip::tcp::resolver>(*mpIOContext);

        SClientConnect local_client_connection;
        local_client_connection.name = mConnectParms.name;
        local_client_connection.mpSocket = std::make_shared<boost::asio::ip::tcp::socket>(*mpIOContext);

        boost::asio::connect(*local_client_connection.mpSocket, mpResolver->resolve(mConnectParms.ipAddress, std::to_string(mConnectParms.portID)));
        std::cout << "Connected to server!" << std::endl;
        mSocketList.emplace_back(std::move(local_client_connection));

        mEnoughConnections = true;

    } catch (std::exception& e) {
        std::cerr << "EXCEPTION HANDLED: " << e.what() << std::endl;
    }
}

int CNetworkTCPIP::Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) {

    int result = 0;
    if(mEnoughConnections) {
        if(outgoing_data.size() > 0) {
            result = boost::asio::write(*mSocketList[0].mpSocket, boost::asio::buffer(outgoing_data));
        } else {
            std::cout << "Write cannot be performed without at least one connection" << std::endl;
        }
    }
    return result;
}

int CNetworkTCPIP::Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) {

    size_t length = 0;

    if(mEnoughConnections) {

        // check the size of the next message
        size_t available_bytes = mSocketList[0].mpSocket->available();
        if( 0 == available_bytes) {
            return 0;
        }

        // read
        std::vector<std::uint8_t> readBuffer(available_bytes);
        boost::system::error_code error;
        length = mSocketList[0].mpSocket->read_some(boost::asio::buffer(readBuffer), error);

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

    std::cout << "Stop called" << std::endl;

    // Close the socket
    mEnoughConnections = false;
    mSocketList[0].mpSocket->shutdown(tcp::socket::shutdown_both);
    mSocketList[0].mpSocket->close();

    mpAcceptor->close();
//    mpAcceptor->cancel();

    mpIOContext.reset();
    mpAcceptor.reset();
    mSocketList[0].mpSocket.reset();
    mpResolver.reset();

//    std::cout << "Network Stopped" << std::endl;
}
