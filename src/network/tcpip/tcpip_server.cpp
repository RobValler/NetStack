/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_server.h"

#include "message_define.h"

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

    mtFunc = std::thread(&CTCPIP_Server::ThreadFunc, this);

    return 0;
}

void CTCPIP_Server::Stop() {

    for(const auto& it : mClientFDList) {
        it->Stop();
    }
    mClientFDList.clear();

    if(-1 == shutdown(mServerFD, SHUT_RDWR) ) {
        std::cerr << strerror(errno) << std::endl;
    }
    close(mServerFD);

    mExitCaller = false;
    mtFunc.join();
}

int CTCPIP_Server::Send(const message::SMessage& msg_data) {

    int body_bytes = 0;
    for(const auto& it : mClientFDList) {
        if(msg_data.ID == it->GetID()) {
            body_bytes = it->Send(msg_data);
        }
    }
    return body_bytes;
}

int CTCPIP_Server::Receive(message::SMessage& msg_data) {

    int body_bytes = 0;
    for(const auto& it : mClientFDList) {
        if(msg_data.ID == it->GetID()) {
            body_bytes = it->Receive(msg_data);
        }
    }
    return body_bytes;
}

int CTCPIP_Server::Connections() {

    int local_num_of_clients = 0;

    for(const auto& it : mClientFDList) {
        if(it->Connected()) {
            local_num_of_clients++;
        }
    }

    return local_num_of_clients;
};

int CTCPIP_Server::ThreadFunc() {

    if(mServerFD >= 0 ) {
        close(mServerFD);
    }

    mServerFD = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerFD < 0) {
        perror("socket");
        return 1;
    }

    // reuse the connection after disconnect
    int opt = 1;
    setsockopt(mServerFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;   // 0.0.0.0
    server_addr.sin_port = htons(mConnectParms.portID);

    if (bind(mServerFD, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(mServerFD);
        return 1;
    }

    while(!mExitCaller) {

        if (listen(mServerFD, 5) < 0) {
            perror("listen");
            close(mServerFD);
            return 1;
        }

        std::cout << "Server now listening on port " << std::to_string(mConnectParms.portID) << std::endl;

        int fd = accept(mServerFD, nullptr, nullptr);
        if (fd < 0) {
            switch(errno) {
                case 22:
                    // ignore invalid argument (22) error
                    break;
                default:
                    std::cerr << "accept error = " <<  strerror(errno) << "(" << errno << ")" << std::endl;
                    break;
            }

            close(mServerFD);
            return 1;
        }

        std::cout << "Server connected to something..." << std::endl;

        // get the ip address of the connecting client
        struct sockaddr_in client_addr;
        socklen_t addr_size = sizeof(struct sockaddr_in);
        (void)getpeername(fd, (struct sockaddr *)&client_addr, &addr_size);
        std::string clientip = inet_ntoa(client_addr.sin_addr);

        // add the client to the client list
        SClientEntryCont local_conn_parms;
        local_conn_parms.client_fd = fd;
        local_conn_parms.ID = 10;
        local_conn_parms.ipaddress = clientip;
        mClientFDList.emplace_back(std::make_shared<CTCPIP_ClientConn>(local_conn_parms));
    }
    return 0;
}
