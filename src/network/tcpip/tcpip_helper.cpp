/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_helper.h"

#include "message_define.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#if 0
int CTCPIP_Helper::Send(const message::SMessage& msg_data) {

    auto foo_data(msg_data);
    foo_data.body_size = (int)foo_data.data_array.size();
    auto header_bytes = write(msg_data.fd, &foo_data.body_size, sizeof(foo_data.body_size));
    auto body_bytes = write(msg_data.fd, &foo_data.data_array[0], foo_data.body_size);
    return body_bytes;
}

int CTCPIP_Helper::Receive(message::SMessage& msg_data) {

    auto foo(msg_data);
    auto hdr_size = sizeof(foo.body_size);
    ssize_t hdr_bytes = recv(msg_data.fd, &foo.body_size, hdr_size, 0);
    if( (hdr_bytes != hdr_size) &&
        (foo.body_size <= 0) ) {
        std::cerr << "Size error" << std::endl;
        return -1;
    }

    //uint16_t msg_size = ntohl(foo.body_size);
    foo.data_array.resize(foo.body_size);
    ssize_t body_bytes = recv(msg_data.fd, &foo.data_array[0], foo.body_size, 0);

    msg_data = foo;
    return body_bytes;
}

int CTCPIP_Helper::Connections() {

    int local_num_of_clients = 0;


    return local_num_of_clients;
};
#endif
