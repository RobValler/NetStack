/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_server_conn.h"

#include "message_define.h"

#ifdef __linux__
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <unistd.h>
#include <iostream>

void CTCPIP_ClientConn::Stop() {

    close(mClientFD);
}

int CTCPIP_ClientConn::Send(const message::SMessage& msg_data) {

    int body_bytes = 0;

    if(mConnected) {
        auto foo_data(msg_data);
        foo_data.body_size = (int)foo_data.data_array.size();
        (void)write(mClientFD, &foo_data.body_size, sizeof(foo_data.body_size));
        body_bytes = write(mClientFD, &foo_data.data_array[0], foo_data.body_size);
        std::cout << "Send called with " << std::to_string(body_bytes) << " number of bytes sent" << std::endl;
    }
    return body_bytes;
}

int CTCPIP_ClientConn::Receive(message::SMessage& msg_data) {

    auto foo(msg_data);
    auto hdr_size = sizeof(foo.body_size);
    ssize_t hdr_bytes = recv(mClientFD, &foo.body_size, hdr_size, 0);
    if( (hdr_bytes != hdr_size) &&
        (foo.body_size <= 0) ) {
        std::cerr << "Size error" << std::endl;
        return -1;
    }

    //uint16_t msg_size = ntohl(foo.body_size);
    foo.data_array.resize(foo.body_size);
    ssize_t body_bytes = recv(mClientFD, &foo.data_array[0], foo.body_size, 0);

    msg_data = foo;
    return body_bytes;
}

bool CTCPIP_ClientConn::Connected() {

    return true;
};
