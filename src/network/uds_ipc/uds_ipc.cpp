/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

// Unix domain socket communication (UDS / IPC)

#include "uds_ipc.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

bool CUDSIPC::Start(const std::string& channel) {

    int mSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (mSocket == -1) {
        perror("socket");
        return false;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, channel.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(mSocket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(mSocket);
        return false;
    }

    return true;
}

void  CUDSIPC::Stop() {

    close(mSocket);
}

int CUDSIPC::Send(const std::string& message) {

    struct msghdr msg{};
    struct iovec iov{};

    // Set up iovec to point to the string data
    iov.iov_base = (void*)message.data();
    iov.iov_len = message.size();

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // No ancillary data
    msg.msg_control = nullptr;
    msg.msg_controllen = 0;

    ssize_t bytes_sent = sendmsg(mSocket, &msg, 0);
    if (bytes_sent == -1) {
        perror("sendmsg");
        return -1;
    }

    return 0;
}
