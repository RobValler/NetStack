/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_client.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

CTCPIP_Client::CTCPIP_Client(const SConnectParms& parms)
    : mConnectParms(parms)
{}

int CTCPIP_Client::Start() {

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(mConnectParms.portID);

    // Connect to localhost
    if (inet_pton(AF_INET, mConnectParms.ipAddress.c_str(), &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    if (connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(client_fd);
        return 1;
    }

    std::cout << "Client connected to server" << std::endl;

    return 0;
}

void CTCPIP_Client::Stop(){

    close(client_fd);
}
