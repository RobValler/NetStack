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

    mExitCaller = false;

    for(const auto& it : mClientFDList) {
        close(it.fd);
    }

    mClientFDList.clear();

    if(-1 == shutdown(server_fd, SHUT_RDWR) ) {
        std::cerr << strerror(errno) << std::endl;
    }
    close(server_fd);

    mExitCaller = false;
    mtFunc.join();
}

int CTCPIP_Server::Send(const message::SMessage& msg_data) {

    int body_bytes = 0;

    if(Connections() > 0) {
        auto foo_data(msg_data);
        foo_data.body_size = (int)foo_data.data_array.size();
        auto header_bytes = write(mClientFDList[0].fd, &foo_data.body_size, sizeof(foo_data.body_size));
        body_bytes = write(mClientFDList[0].fd, &foo_data.data_array[0], foo_data.body_size);
    }
    return body_bytes;
}

int CTCPIP_Server::Receive(message::SMessage& msg_data) {

    auto foo(msg_data);
    auto hdr_size = sizeof(foo.body_size);
    ssize_t hdr_bytes = recv(mClientFDList[0].fd, &foo.body_size, hdr_size, 0);
    if( (hdr_bytes != hdr_size) &&
        (foo.body_size <= 0) ) {
        std::cerr << "Size error" << std::endl;
        return -1;
    }

    //uint16_t msg_size = ntohl(foo.body_size);
    foo.data_array.resize(foo.body_size);
    ssize_t body_bytes = recv(mClientFDList[0].fd, &foo.data_array[0], foo.body_size, 0);

    msg_data = foo;
    return body_bytes;
}

int CTCPIP_Server::Connections() {

    int local_num_of_clients = mClientFDList.size();

    return local_num_of_clients;
};

int CTCPIP_Server::ThreadFunc() {

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

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;   // 0.0.0.0
    server_addr.sin_port = htons(mConnectParms.portID);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    while(!mExitCaller) {

        if (listen(server_fd, 5) < 0) {
            perror("listen");
            close(server_fd);
            return 1;
        }

        std::cout << "Server listening on port " << std::to_string(mConnectParms.portID) << std::endl;

        int fd = accept(server_fd, nullptr, nullptr);
        if (fd < 0) {
            switch(errno) {
                case 22:
                    // ignore invalid argument (22) error
                    break;
                default:
                    std::cerr << "accept error = " <<  strerror(errno) << "(" << errno << ")" << std::endl;
                    break;
            }

            close(server_fd);
            return 1;
        }

        std::cout << "Server connected to something..." << std::endl;

        // add the client to the client list
        struct sockaddr_in client_addr;
        socklen_t addr_size = sizeof(struct sockaddr_in);
        int res = getpeername(fd, (struct sockaddr *)&client_addr, &addr_size);
        //char *clientip = new char[20];
        std::string clientip = inet_ntoa(client_addr.sin_addr);

        SClientEntryCont cont;
        cont.fd = fd;
        cont.ipaddress = clientip;
        mClientFDList.emplace_back(std::move(cont));
    }
    return 0;
}
