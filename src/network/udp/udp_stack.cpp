/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "udp_stack.h"

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

CUDP_Stack::CUDP_Stack(const SConnectParms& parms)
    : mConnectParms(parms)
{ /* do nothing */ }


int CUDP_Stack::Start() {

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    // LOCAL NODE
    mLocalSockFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (mLocalSockFD < 0) {
#ifdef __linux__
    perror("mLocalSockFD socket error");
#elif _WIN32
        WSACleanup();
#endif
        return 1;
    }

    mLocalAddr.sin_family = AF_INET;
    mLocalAddr.sin_addr.s_addr = INADDR_ANY;
    mLocalAddr.sin_port = htons(mConnectParms.portLocalID);

    if (bind(mLocalSockFD, (sockaddr*)&mLocalAddr, sizeof(mLocalAddr)) < 0) {
#ifdef __linux__
        perror("mLocalSockFD bind");
        close(mLocalSockFD);
#elif _WIN32

        closesocket(mLocalSockFD);
        WSACleanup();
#endif
        return 1;
    }

    mRemoteAddr.sin_family = AF_INET;
    mRemoteAddr.sin_port = htons(mConnectParms.portRemoteID);
    auto res = inet_pton(AF_INET, mConnectParms.ipAddress.c_str(), &mRemoteAddr.sin_addr);
    if(1 != res) {

        // return value is wierd
        std::cerr << "portRemoteID inet_pton error: " << std::strerror(errno) << "\n";
        return 1;
    }

    return 0;
}

void CUDP_Stack::Stop() {

#ifdef __linux__
    close(mLocalSockFD);
#elif _WIN32
    closesocket(mLocalSockFD);
    WSACleanup();
#endif
}

int CUDP_Stack::Send(const message::SMessage& msg_data) {

    int body_bytes = sendto(mLocalSockFD,
                            &msg_data.data_array[0],
                            msg_data.data_array.size(),
                            0, (sockaddr*)&mRemoteAddr,
                            sizeof(mRemoteAddr));
    if(body_bytes <= 0) {
        std::cerr << "server send error " << strerror(errno) << std::endl;
    }

    return body_bytes;
}

int CUDP_Stack::Receive(message::SMessage& msg_data) {

    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    std::vector<std::uint8_t> data(1024);
    ssize_t body_bytes = recvfrom(mLocalSockFD,
                                  &data[0],
                                  data.size(),
                                  0, (sockaddr*)&clientAddr,
                                  &clientLen);
    if(body_bytes <= 0) {
        std::cerr << "server receive error " << strerror(errno) << std::endl;
    } else {

        msg_data.data_array.resize(body_bytes);
        msg_data.data_array.assign(data.begin(), data.end());
    }

    return body_bytes;
}

int CUDP_Stack::Connections() {

    return 1; // default to 1, we dont actually have a connection
};
