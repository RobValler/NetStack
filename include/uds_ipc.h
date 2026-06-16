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

    int Send(const std::string& message);
    int Receive(std::string& message);
    void Get(const std::string& channel, std::string& message);

private:
    bool Connect(const std::string& channel);
    void Disconnect();

    int mSocket{-1};
    bool mIsConnected{false};
};
