/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <string>

class CUDSIPC {
public:
    CUDSIPC() =default;
    ~CUDSIPC() =default;

    bool Start(const std::string& channel);
    void Stop();
    int Send(const std::string& message);

private:
    int mSocket;
};
