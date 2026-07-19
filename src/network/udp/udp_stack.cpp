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


CUDP_Stack::CUDP_Stack()
    : mSendRec(std::make_unique<CRevSend>())
{/**/}

CUDP_Stack::~CUDP_Stack()
{/**/}

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

    int opt = 1;
    setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(mSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    mBroadcastAddr.sin_family = AF_INET;
    mBroadcastAddr.sin_port = htons(mConnectParms.portLocalID);
    inet_pton(AF_INET, mConnectParms.broadcastIpAddress.c_str(), &mBroadcastAddr.sin_addr.s_addr);

    // REMOTE RECEIVER
    mRemoteAddr.sin_family = AF_INET;
    mRemoteAddr.sin_port = htons(mConnectParms.portRemoteID);
    mRemoteAddr.sin_addr.s_addr = INADDR_ANY;
    // if(1 != inet_pton(AF_INET, mConnectParms.broadcastIpAddress.c_str(), &mRemoteAddr.sin_addr)) {

    //     std::cerr << "[UDP] portRemoteID inet_pton error: " << std::strerror(errno) << "\n";
    //     return 1;
    // }

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

    //int body_bytes = mSendRec->MySend(mSocket, msg_data, mBroadcastAddr);
    int body_bytes = mSendRec->MySend(mSocket, msg_data);
    if(body_bytes <= 0) {
        error_log("[UDP] Send error");
    }
    return body_bytes;
}

int CUDP_Stack::Receive(message::SMessage& msg_data) {

    int body_bytes = mSendRec->MyReceive(mSocket, msg_data);
    if(body_bytes <= 0) {
        std::cerr << "[UDP] Receive error " << strerror(errno) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    } else {

        // get the senders IP Address
        char local_ip_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &mRemoteAddr.sin_addr, local_ip_address, INET_ADDRSTRLEN);
        msg_data.mIpAddress = local_ip_address;

        // get the senders Port
        msg_data.mPort = ntohs(mRemoteAddr.sin_port);
    }

    return body_bytes;
}
