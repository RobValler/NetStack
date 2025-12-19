/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "udp_client.h"

#include "message_define.h"

#ifdef __linux__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#error "OS not supported"
#endif

#include <iostream>
#include <cstring>

CUDP_Client::CUDP_Client(const SConnectParms& parms)
    : mConnectParms(parms)
{ /* do nothing */ }

CUDP_Client::~CUDP_Client()
{ /* do nothing */ }

int CUDP_Client::Start() {

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    mSockFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSockFD < 0) {
        perror("socket");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    mServerAddr.sin_family = AF_INET;
    mServerAddr.sin_port = htons(mConnectParms.portID);
    inet_pton(AF_INET, mConnectParms.ipAddress.c_str(), &mServerAddr.sin_addr);

    return 0;
}

void CUDP_Client::Stop() {

#ifdef __linux__
    close(mSockFD);
#elif _WIN32
    closesocket(sock);
    WSACleanup();
#endif
}

int CUDP_Client::Send(const message::SMessage& msg_data) {

    int body_bytes = sendto(mSockFD,
                            &msg_data.data_array[0],
                            msg_data.data_array.size(),
                            0, (sockaddr*)&mServerAddr,
                            sizeof(mServerAddr));

    if(body_bytes <= 0) {
        std::cerr << "server receive error " << strerror(errno) << std::endl;
    }

    return body_bytes;
}

int CUDP_Client::Receive(message::SMessage& msg_data) {

    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    msg_data.data_array.resize(1024);
    ssize_t body_bytes = recvfrom(mSockFD,
                                  &msg_data.data_array[0],
                                  msg_data.data_array.size(),
                                  0, (sockaddr*)&clientAddr,
                                  &clientLen);

    if(body_bytes <= 0) {
        std::cerr << "server receive error " << strerror(errno) << std::endl;
    }

    return body_bytes;
}

int CUDP_Client::Connections() {

    return 1;
};
