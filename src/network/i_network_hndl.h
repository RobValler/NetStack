/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef I_NETWORK_HNDL__H
#define I_NETWORK_HNDL__H

#include <vector>
#include <cstdint>

struct SNetIF;

class INetworkHndl {
public:
    ~INetworkHndl(){}
    virtual int Start() =0;
    virtual int Send(const SNetIF&, const std::vector<std::uint8_t>&) =0;
    virtual int Receive(const SNetIF&, std::vector<std::uint8_t>&) =0;
    virtual bool IsConnected() =0;
    virtual void Stop() =0;
};

#endif // I_NETWORK_HNDL__H
