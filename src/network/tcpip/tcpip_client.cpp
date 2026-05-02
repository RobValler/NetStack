/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_client.h"

#include "message_define.h"
#include "encrypt_tls.h"


#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>

CTCPIP_Client::CTCPIP_Client(const STCPIPClientParms& parms)
    : mConnectParms(parms) {

    SEntryptILSData data;
    data.cert = parms.cert;
    data.pkey = parms.pkey;
    mpTLS = std::make_shared<EncryptTLS>(data);
}

int CTCPIP_Client::Start() {



    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket");
        return 1;
    }

    // configuration
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(mConnectParms.portID);   
    if (inet_pton(AF_INET, mConnectParms.remoteIpAddress.c_str(), &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(0); // 0 = let OS choose port

    inet_pton(AF_INET, mConnectParms.localIpAddress.c_str(), &local_addr.sin_addr);

    if (bind(client_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("local bind");
        return 1;
    }

    // Connect to localhost
    // we apply a re-try strategy here
    mIsConnected = false;
    for(int retry_counter = 0; retry_counter < mConnectParms.maxConnectRetryAttempts; ++retry_counter) {

        std::cout << "Attempting to connect to " << mConnectParms.remoteIpAddress << std::endl;
        if (0 == connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {

            if(mpTLS->Connect(client_fd)) {
                mIsConnected = true;

            } else {
                //perror("tcpip client TLS connect error");
                return 1;
            }

            break;

        } else {
            perror("tcpip client connect error");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    if(!mIsConnected) {
        close(client_fd);
        return 1;
    }

    std::cout << "Client: Connected to remote server (" << mConnectParms.remoteIpAddress << ")" << std::endl;

    return 0;
}

void CTCPIP_Client::Stop(){

    close(client_fd);
}

int CTCPIP_Client::Send(const message::SMessage& msg_data) {

#if 1
    return mpTLS->Send(msg_data);
#else
    auto foo_data(msg_data);
    foo_data.body_size = (int)foo_data.mMsgPayload.size();
    auto header_bytes = write(client_fd, &foo_data.body_size, sizeof(foo_data.body_size));
    auto body_bytes = write(client_fd, &foo_data.mMsgPayload[0], foo_data.body_size);
    return body_bytes;
#endif
}

int CTCPIP_Client::Receive(message::SMessage& msg_data) {

#if 1
    return mpTLS->Receive(msg_data);
#else
    auto foo(msg_data);
    auto hdr_size = sizeof(foo.body_size);
    ssize_t hdr_bytes = recv(client_fd, &foo.body_size, hdr_size, 0);
    if( (hdr_bytes != hdr_size) &&
        (foo.body_size <= 0) ) {
        std::cerr << "Size error" << std::endl;
        return -1;
    }

    //uint16_t msg_size = ntohl(foo.body_size);
    foo.mMsgPayload.resize(foo.body_size);
    ssize_t body_bytes = recv(client_fd, &foo.mMsgPayload[0], foo.body_size, 0);

    msg_data = foo;
    return body_bytes;
#endif
}

bool CTCPIP_Client::Connection() {

    return mIsConnected;
};
