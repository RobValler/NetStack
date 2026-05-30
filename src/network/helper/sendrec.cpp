/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "sendrec.h"

#include "message_define.h"

#ifdef __linux__
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include <cstring>

int MySend(int sock, const message::SMessage& msg, const sockaddr_in& addr) {

    int total_bytes_sent = 0;
    int expected_bytes_sent = sizeof(std::uint32_t);

    // send the header
    std::uint32_t head_len = htonl(msg.mMsgPayload.size());
    const uint8_t* p_head_len = reinterpret_cast<const uint8_t*>(&head_len);
    while (total_bytes_sent < expected_bytes_sent) {
        int bytes_sent = sendto(sock,
                                p_head_len + total_bytes_sent,
                                expected_bytes_sent - total_bytes_sent,
                                0,
                                (sockaddr*)&addr,
                                sizeof(addr));

        if (bytes_sent <= 0) {
            return bytes_sent;
        }
        total_bytes_sent += bytes_sent;
    }

    // send the payload
    total_bytes_sent = 0;
    expected_bytes_sent = ntohl(head_len);
    while(total_bytes_sent < expected_bytes_sent) {

        int bytes_sent = sendto(sock,
                                msg.mMsgPayload.data() + total_bytes_sent,
                                expected_bytes_sent - total_bytes_sent,
                                0,
                                (sockaddr*)&addr,
                                sizeof(addr));
        if(bytes_sent <= 0) {
            return bytes_sent;
        }
        total_bytes_sent += bytes_sent;
    }

    return total_bytes_sent;
}

int MyReceive(int sock, message::SMessage& msg) {

    std::uint32_t  total_bytes_received = 0;
    std::uint32_t  expected_bytes_sent = sizeof(std::uint32_t);

    // receive the header
    std::uint32_t head_len;
    auto p_head_len = reinterpret_cast<uint8_t*>(&head_len);
    while (total_bytes_received < expected_bytes_sent) {

        int bytes_received = recv(sock, p_head_len + total_bytes_received, expected_bytes_sent - total_bytes_received, 0);
        if (bytes_received <= 0) {
            return bytes_received;
        }
        total_bytes_received += bytes_received;
    }

    // receive the payload
    msg.body_size = ntohl(head_len);
    total_bytes_received = 0;
    msg.mMsgPayload.resize(msg.body_size);
    while(total_bytes_received < msg.body_size) {

        int bytes_received = recv(sock, &msg.mMsgPayload[0] + total_bytes_received, msg.body_size - total_bytes_received, 0);
        if(bytes_received <= 0) {
            return bytes_received;
        }
        total_bytes_received += bytes_received;
    }

    return total_bytes_received;
}
