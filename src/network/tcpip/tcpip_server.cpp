/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

CTCPIP_Server::CTCPIP_Server(const SConnectParms& parms)
    : mConnectParms(parms)
{}

int CTCPIP_Server::Start() {

    if(server_fd >= 0 ) {
        close(server_fd);
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    // reuse the connection after disconnect
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;   // 0.0.0.0
    addr.sin_port = htons(mConnectParms.portID);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port " << std::to_string(mConnectParms.portID) << std::endl;

    client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    std::cout << "Server connected to something..." << std::endl;

    return 0;
}

void CTCPIP_Server::Stop() {

    close(client_fd);
    close(server_fd);
}
