/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "posix_mq_node.h"

#include "message_define.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <cstring>
//#include <iostream>

#define MAX_MSG_SIZE 1024


CPOSIX_MQ_Node::CPOSIX_MQ_Node(const SConnectParms& parms)
    : mConnectParms(parms)
{ /**/ }

int CPOSIX_MQ_Node::Start() {



    return 0;
}

int CPOSIX_MQ_Node::Send(const message::SMessage& data) {

    mMQSend = mq_open(mConnectParms.channel_send.c_str(), O_WRONLY);
    if (mMQSend == (mqd_t)-1) {
        std::string error_txt = "mq_open error for " + mConnectParms.channel_send;

        perror(error_txt.c_str());
        return(0);
    }

    int bytes = data.data_array.size();
    if (mq_send(mMQSend, (const char *)&data.data_array[0], (size_t)bytes, 0) == -1) {
        perror("mq_send");
        bytes = -1;
    }

    return bytes;
}

int CPOSIX_MQ_Node::Receive(message::SMessage& data) {

    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];

    /* Queue attributes */
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    /* Open or create the queue */
    mMQRecv = mq_open(mConnectParms.channel_recv.c_str(), O_CREAT | O_RDONLY, 0644, &attr);
    if (mMQRecv == (mqd_t)-1) {
        std::string error_txt = "mq_open error for " + mConnectParms.channel_send;
        perror(error_txt.c_str());
        return(0);
    }

    /* This call blocks until a message is available */
    auto bytes = mq_receive(mMQRecv, buffer, MAX_MSG_SIZE, NULL);
    if (bytes <= 0) {
        perror("mq_receive");
    } else {
        //std::vector<char> vec(arr, arr + sizeof(arr) - 1); // Exclude the null terminator
        data.data_array.resize(bytes);
        data.data_array.assign(&buffer[0], &buffer[0] + bytes);
    }

    return bytes;
}

int CPOSIX_MQ_Node::Connections() {

    int local_num_of_clients = 1; // fake a connection


    return local_num_of_clients;
}

void CPOSIX_MQ_Node::Stop(){

    mq_close(mMQSend);
    mq_close(mMQRecv);
    mq_unlink(mConnectParms.channel_recv.c_str());
    mq_unlink(mConnectParms.channel_send.c_str());
}
