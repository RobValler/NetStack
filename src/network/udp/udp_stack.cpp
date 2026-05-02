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
#include <thread>
#include <chrono>
#include <iostream>


int CUDP_Stack::Start(const SUDPParms& parms) {

    mConnectParms = parms;

    // LOCAL NODE
    mLocalSockFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (mLocalSockFD < 0) {
#ifdef __linux__
    perror("mLocalSockFD socket error");
#endif
        return 1;
    }

    if(mConnectParms.broadCastSender) {

        // Enable broadcast
        int broadcastEnable = 1;
        if (setsockopt(mLocalSockFD, SOL_SOCKET, SO_BROADCAST,
                       &broadcastEnable, sizeof(broadcastEnable)) < 0) {
            perror("setsockopt");
            return 1;
        }
    }
    mLocalAddr.sin_family = AF_INET;
    mLocalAddr.sin_port = htons(mConnectParms.portLocalID);
    mLocalAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(mLocalSockFD, (sockaddr*)&mLocalAddr, sizeof(mLocalAddr)) < 0) {
#ifdef __linux__
        perror("[UDP] mLocalSockFD bind");
        close(mLocalSockFD);
#elif _WIN32

        closesocket(mLocalSockFD);
        WSACleanup();
#endif
        return 1;
    }

    mRemoteAddr.sin_family = AF_INET;
    mRemoteAddr.sin_port = htons(mConnectParms.portRemoteID);
    if(1 != inet_pton(AF_INET, mConnectParms.remoteIpAddress.c_str(), &mRemoteAddr.sin_addr)) {

        std::cerr << "[UDP] portRemoteID inet_pton error: " << std::strerror(errno) << "\n";
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
                            &msg_data.mMsgPayload[0],
                            msg_data.mMsgPayload.size(),
                            0, (sockaddr*)&mRemoteAddr,
                            sizeof(mRemoteAddr));
    if(body_bytes <= 0) {
        std::cerr << "[UDP] Send error " << strerror(errno) << std::endl;
    }

    return body_bytes;
}

int CUDP_Stack::Receive(message::SMessage& msg_data) {

    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    std::vector<std::uint8_t> data(4096);
    ssize_t body_bytes = recvfrom(mLocalSockFD,
                                  &data[0],
                                  data.size(),
                                  0, (sockaddr*)&clientAddr,
                                  &clientLen);
    if(body_bytes <= 0) {
        std::cerr << "[UDP] Receive error " << strerror(errno) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    } else {

        // fetch payload
        msg_data.mMsgPayload.resize(body_bytes);
        msg_data.mMsgPayload.assign(data.begin(), data.begin() + body_bytes);

        // get the senders IP Address
        char local_ip_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, local_ip_address, INET_ADDRSTRLEN);
        msg_data.mIpAddress = local_ip_address;

        // get the senders Port
        msg_data.mPort = ntohs(clientAddr.sin_port);
    }

    return body_bytes;
}
