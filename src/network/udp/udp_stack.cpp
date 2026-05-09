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

#include "error_log.h"
#include "sendrec.h"

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

    // SOCKET
    mSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSocket < 0) {
        error_log("[UDP] mLocalSockFD socket error");
        Stop();
        return 1;
    }

    if(mConnectParms.broadcaster) {

        // Enable broadcast
        int broadcastEnable = 1;
        if (setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
            perror("setsockopt");
            return 1;
        }
    }

    mBroadcastAddr.sin_family = AF_INET;
    mBroadcastAddr.sin_port = htons(mConnectParms.portLocalID);
    inet_pton(AF_INET, mConnectParms.broadcastIpAddress.c_str(), &mBroadcastAddr.sin_addr);

    // REMOTE RECEIVER
    mRemoteAddr.sin_family = AF_INET;
    mRemoteAddr.sin_port = htons(mConnectParms.portRemoteID);
    if(1 != inet_pton(AF_INET, mConnectParms.broadcastIpAddress.c_str(), &mRemoteAddr.sin_addr)) {

        std::cerr << "[UDP] portRemoteID inet_pton error: " << std::strerror(errno) << "\n";
        return 1;
    }

    if (bind(mSocket, (sockaddr*)&mRemoteAddr, sizeof(mRemoteAddr)) < 0) {

        error_log("[UDP] mRemoteSockFD bind error");
        Stop();
        return 1;
    }

    return 0;
}

void CUDP_Stack::Stop() {

#ifdef __linux__
    close(mSocket);
#elif _WIN32
    closesocket(mLocalSockFD);
    //WSACleanup();
#endif
}

int CUDP_Stack::Send(const message::SMessage& msg_data) {

#if 1
    int body_bytes = MySend(mSocket, msg_data, mBroadcastAddr);
#else
    int body_bytes = sendto(mSocket,
                            &msg_data.mMsgPayload[0],
                            msg_data.mMsgPayload.size(),
                            0,
                            (sockaddr*)&mBroadcastAddr,
                            sizeof(mBroadcastAddr));
#endif
    if(body_bytes <= 0) {
        error_log("[UDP] Send error");
    }
    return body_bytes;
}

int CUDP_Stack::Receive(message::SMessage& msg_data) {

//    std::vector<std::uint8_t> data(4096);
#if 1
    int body_bytes = MyReceive(mSocket, msg_data, mRemoteAddr);
#else
    ssize_t body_bytes = recv(mSocket, &data[0], sizeof(data), 0);
    // ssize_t body_bytes = recvfrom(mSocket,
    //                               &data[0],
    //                               data.size(),
    //                               0,
    //                               (sockaddr*)&mRemoteAddr,
    //                               sizeof(mRemoteAddr));
#endif

    if(body_bytes <= 0) {
        std::cerr << "[UDP] Receive error " << strerror(errno) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    } else {

        // fetch payload
        // msg_data.mMsgPayload.resize(body_bytes);
        // msg_data.mMsgPayload.assign(data.begin(), data.begin() + body_bytes);

        // get the senders IP Address
        char local_ip_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &mRemoteAddr.sin_addr, local_ip_address, INET_ADDRSTRLEN);
        msg_data.mIpAddress = local_ip_address;

        // get the senders Port
        msg_data.mPort = ntohs(mRemoteAddr.sin_port);
    }

    return body_bytes;
}
