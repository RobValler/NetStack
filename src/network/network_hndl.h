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

#include "network_connect_parms.h"
#include "serialise.h"

namespace google::protobuf{ class Message; }

#include <memory>
#include <thread>

class INetworkHndl;
class CEncrypt;
struct SConnectParms;

class CNetworkHndl
{
public:
    CNetworkHndl(const SConnectParms& parms);
    ~CNetworkHndl();
    
    void Start();
    void Stop();
    bool IsConnected();
    int Receive(const SNetIF& operater, google::protobuf::Message& proto_message);
    int Send(const SNetIF& operater, const google::protobuf::Message& proto_message);

private:
    void ThreadFuncServer();

    SConnectParms mParms;
    std::thread mtServer;    
    std::unique_ptr<INetworkHndl> mpNetworkConnection;
    std::unique_ptr<CSerial> mpSerialiser;
    std::unique_ptr<CEncrypt> mpEncrypt;
    bool mIsThereAConnection{false};
};

#endif // NETWORK_HNDL__H
