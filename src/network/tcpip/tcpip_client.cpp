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
#include "error_log.h"
#include "logger.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <thread>
#include <chrono>

int CTCPIP_Client::Start(const STCPIPClientParms& parms) {

    mConnectParms = parms;

    SEntryptILSData data;
    data.cert = parms.cert;
    data.pkey = parms.pkey;
    mpTLS = std::make_shared<EncryptTLS>(data);

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

        CLogger::Log("[TCPIP Client] Attempting to connect to " + mConnectParms.remoteIpAddress);
        if (connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == 0) {

            if(mpTLS->Connect(client_fd)) {
                mIsConnected = true;                

            } else {
                error_log("[TCPIP Client] TLS connect error : ");
                return 1;
            }
            break;

        } else {
            error_log("[TCPIP Client] connect error : ");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    if(!mIsConnected) {
        close(client_fd);
        return 1;
    }

    CLogger::Log("[TCPIP Client] Connected to remote server (" + mConnectParms.remoteIpAddress + ")");

    return 0;
}

void CTCPIP_Client::Stop(){

    close(client_fd);
}

int CTCPIP_Client::Send(const message::SMessage& msg_data) {

    return mpTLS->Send(msg_data);
}

int CTCPIP_Client::Receive(message::SMessage& msg_data) {

    return mpTLS->Receive(msg_data);
}

bool CTCPIP_Client::Connection() {

    return mIsConnected;
};
