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
    boost::asio::io_context io_context;
    udp::socket send_socket(io_context, udp::endpoint(udp::v4(), 0)); // Local socket

    while( true ) {

        try {
            // Create a resolver for the receiving endpoint
            udp::resolver resolver(io_context);
            udp::resolver::results_type endpoints = resolver.resolve(udp::v4(), "127.0.0.1", "1101"); // Replace with your server address and port

            std::string message = "Hello, UDP server!";
            send_socket.send_to(boost::asio::buffer(message), *endpoints.begin()); // Send the message

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        } catch (std::exception& e) {
            std::cerr << "EXCEPTION HANDLED: " << e.what() << std::endl;
        }
    } // while

    mIsInitialised = true;
}

void CNetwork_UDP::Client() {

    if(config::gMinPortNumber > mConnectParms.portID) {
        std::cout << "Warning: Attempted to access privilaged port number (below 1024). Check permissions!" << std::endl;
    }
    std::cout << "UDP Client started" << std::endl;

    std::size_t length;

    boost::asio::io_context io_context;

    udp::socket receive_socket(io_context, udp::endpoint(udp::v4(), 1101)); // Listening on the same port
    char recv_buffer[1024];
    udp::endpoint sender_endpoint;

    while( true ) {

        try {
            std::size_t length = receive_socket.receive_from(boost::asio::buffer(recv_buffer), sender_endpoint);
            std::cout << "Received message: "
                      << std::string(recv_buffer, length)
                      << " from "
                      << sender_endpoint.address().to_string()
                      << ":" << sender_endpoint.port()
                      << std::endl;

        } catch (std::exception& e) {
            std::cerr << "EXCEPTION HANDLED: " << e.what() << std::endl;
        }
    }

    mIsInitialised = true;
}

int CNetwork_UDP::Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) {

    if(!mIsInitialised) {
        return 0;
    }

    int length = 0;



    return length;
}


int CNetwork_UDP::Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) {

    if(!mIsInitialised) {
        return 0;
    }

    int length = 0;



    return length;
}

void CNetwork_UDP::Stop() {

    // Close the socket
    mpSocket->shutdown(tcp::socket::shutdown_both);
    mpSocket->close();

    mpIOContext.reset();
    mpSocket.reset();

}
