/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef NETWORK_POSIX__H
#define NETWORK_POSIX__H

class CNetwork_POSIX
{
public:
    CNetwork_POSIX();
    ~CNetwork_POSIX();

    void Server();
    void Client();
    int Receive();
    int Send();
    void Stop();

private:
    void ThreadFuncServer();

};

#endif // NETWORK_POSIX__H
