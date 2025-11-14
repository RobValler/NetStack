/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef NETWORK_HNDL__H
#define NETWORK_HNDL__H

#include "serialise.h"
#include "network_connect_parms.h"

#include <memory>
#include <thread>

struct SConnectParms;
class CNetworkTCPIP;

namespace google::protobuf { class Message; }

class CNetworkHndl
{
public:
    CNetworkHndl(const SConnectParms& parms);
    ~CNetworkHndl();
    
    void Start();
    void Stop();
    bool IsConnected();
    int Receive(google::protobuf::Message& proto_message);
    int Send(const google::protobuf::Message& proto_message);

private:
    SConnectParms mParms;
    std::thread mtServer;
    void ThreadFuncServer();
    std::unique_ptr<CNetworkTCPIP> mpNetworkConnection;
    std::unique_ptr<CSerial> mpSerialiser;
};

#endif // NETWORK_HNDL__H
