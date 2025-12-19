/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "udp_server.h"

#include "message_define.h"

#ifdef __linux__
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#error "OS not supported"
#endif

#include <cstring>
#include <iostream>

CUDP_Server::CUDP_Server(const SConnectParms& parms)
    : mConnectParms(parms)
{ /* do nothing */ }


int CUDP_Server::Start() {

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    mSockFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSockFD < 0) {
        perror("server socket error");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    mServerAddr.sin_family = AF_INET;
    mServerAddr.sin_addr.s_addr = INADDR_ANY;
    mServerAddr.sin_port = htons(mConnectParms.portID);

    if (bind(mSockFD, (sockaddr*)&mServerAddr, sizeof(mServerAddr)) < 0) {
        perror("server bind");
#ifdef __linux__
        close(mSockFD);
#elif _WIN32
        closesocket(sock);
        WSACleanup();
#endif
        return 1;
    }
    return 0;
}

void CUDP_Server::Stop() {

#ifdef __linux__
    close(mSockFD);
#elif _WIN32
    closesocket(sock);
    WSACleanup();
#endif
}

int CUDP_Server::Send(const message::SMessage& msg_data) {

    auto foo_data(msg_data);
    int body_bytes = sendto(mSockFD,
                            &foo_data.data_array[0],
                            foo_data.data_array.size(),
                            0, (sockaddr*)&mServerAddr,
                            sizeof(mServerAddr));
    if(body_bytes <= 0) {
        std::cerr << "server send error " << strerror(errno) << std::endl;
    }

    return body_bytes;
}

int CUDP_Server::Receive(message::SMessage& msg_data) {

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

int CUDP_Server::Connections() {

    return 1;
};
