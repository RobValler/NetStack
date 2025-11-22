/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "network_posix.h"

#include "configuration.h"

#include <error.h>
#include <string.h>

#include <iostream>
#include <chrono>
#include <thread>

namespace
{
    constexpr std::uint32_t msg_size = 1024;
    constexpr std::uint32_t max_msg = 10;
    constexpr std::uint32_t permission = 0644;
}


CNetwork_POSIX::CNetwork_POSIX(const SConnectParms& parms)
    : mConnectParms(parms)
{ /* nothing */ }

CNetwork_POSIX::~CNetwork_POSIX()
{ /* nothing */ }

bool CNetwork_POSIX::Server() {

    bool result = false;

    std::cout << "POSIX Server started" << std::endl;

    // create the posix mq channel
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = max_msg;
    attr.mq_msgsize = msg_size;
    attr.mq_curmsgs = 0;

    //mode_t mode  = S_IRWXU | S_IRWXG | S_IRWXO;
    int o_flag = O_CREAT | O_RDWR  ;//| O_NONBLOCK ;

    mPOSIXName = config::gPOSIXMQChannelName + "server";
    mPOSIXChannel = mq_open(mPOSIXName.data(), o_flag, permission, &attr);

    if(-1 != mPOSIXChannel) {
        std::cout << "server channel " << mPOSIXName << " created" << std::endl;
        mIsConnected = true;
        result = true;

    } else {
        std::cout << "server open failed: " << mPOSIXName << " Error number " << strerror(errno) << std::endl;
    }

    while( !mExitCalled ) {
        std::this_thread::sleep_for(std::chrono::seconds(1000));

    } // while

    return true;
}

bool CNetwork_POSIX::Client() {

    std::cout << "POSIX Client started" << std::endl;

    for(int retry_index = 1; retry_index <= config::gMaxNumOfRetryAttempts; ++retry_index)
    {
        std::cout << "client channel " << mPOSIXName << " connect attempt " << std::to_string(retry_index) << std::endl;
        if(config::gMaxNumOfRetryAttempts == retry_index) {
            std::cout << "max channel connect attempts reached" << std::endl;
            return false;
        }

        mPOSIXName = config::gPOSIXMQChannelName + "server";
        mPOSIXChannel = mq_open(mPOSIXName.data(), O_WRONLY);

        if(-1 != mPOSIXChannel) {
            mIsConnected = true;

            std::cout << "created channel " << mPOSIXName << std::endl;
            break;
        } else {
            std::cout << "client open failed: " << mPOSIXName << " Error: " << strerror(errno) << std::endl;
            mExitCalled = true;
        }

        std::this_thread::sleep_for( std::chrono::seconds(1) );
    }

    while( !mExitCalled ) {
        std::this_thread::sleep_for(std::chrono::seconds(1000));

    } // while

    return true;
}

int CNetwork_POSIX::Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) {

    unsigned int priority = 1;
    int result = mq_send(mPOSIXChannel, (const char*)outgoing_data.data(), outgoing_data.size(), priority);
    if(result < 0) {
        std::cout << mPOSIXChannel << " send failed = " << strerror(errno) << std::endl;
    } else {
        result = outgoing_data.size();
    }
    return result;
}

int CNetwork_POSIX::Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) {

    int numOfBytesRead = 0;
    unsigned int priority = 1;

    // blocking read
    outgoing_data.resize(1024);
    numOfBytesRead = mq_receive(mPOSIXChannel, (char *)&outgoing_data[0], 1024, &priority);

    if(numOfBytesRead <= 0) {
        std::cout << mPOSIXChannel << " send failed = " << strerror(errno) << std::endl;
    }

    return numOfBytesRead;
}

bool CNetwork_POSIX::IsConnected() {

    return mIsConnected;
}

void CNetwork_POSIX::Stop() {

    if(0 != mq_close(mPOSIXChannel)){
        return;
    }
    if(0 != mq_unlink(mPOSIXName.data())) {
        return;
    }
}
