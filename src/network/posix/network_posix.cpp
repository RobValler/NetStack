/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "network_posix.h"

#include <iostream>

CNetwork_POSIX::CNetwork_POSIX()
{ /* nothing */ }

CNetwork_POSIX::~CNetwork_POSIX()
{ /* nothing */ }

void CNetwork_POSIX::Server() {

    std::cout << "POSIX Server started" << std::endl;

}

void CNetwork_POSIX::Client() {

    std::cout << "POSIX Client started" << std::endl;
}

int CNetwork_POSIX::Send() {

    int result = 0;

    return result;
}

int CNetwork_POSIX::Receive() {

    int length = 0;

    return length;
}

void CNetwork_POSIX::Stop() {

}
