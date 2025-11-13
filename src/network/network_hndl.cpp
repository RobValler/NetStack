/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

// wrapper for actual networking stack
#include "network_hndl.h"

#include <google/protobuf/message.h>

#include "network_tcpip.h"

#include <iostream>

CNetworkHndl::CNetworkHndl(const SConnectParms& parms)
    : mParms(parms)
    , mNetworkConnection( std::make_shared<CNetworkTCPIP>(parms) )
{ /* nothing */ }

void CNetworkHndl::Start() {

    // blocking call
    //ThreadFuncServer();
    mtServer = std::thread(&CNetworkHndl::ThreadFuncServer, this);
}

void CNetworkHndl::Stop() {

    mNetworkConnection->Stop();
    mtServer.join();
    std::cout << "Network stopped " << std::endl;
}

bool CNetworkHndl::IsConnected() {
    return mNetworkConnection->IsConnected();
}

int CNetworkHndl::Receive(google::protobuf::Message& proto_message) {

    std::vector<char> outgoing_data;
    int outgoing_size = mNetworkConnection->Receive(outgoing_data); // blocking
    Deserialise(outgoing_data, proto_message, outgoing_size);
    return outgoing_size;
}

int CNetworkHndl::Send(const google::protobuf::Message& proto_message) {

    int result = 0;

    std::vector<char> outgoing_data;
    int outgoing_size{0};
    if(Serialise(proto_message, outgoing_data, outgoing_size)) {
        result = mNetworkConnection->Send(outgoing_data);
    }

    return result;
}

void CNetworkHndl::ThreadFuncServer() {

    switch(mParms.type)
    {
        case ECommsType::ETypeServer:
            mNetworkConnection->Server();    // this should be a blocking call
            break;
        case ECommsType::ETypeClient:
            mNetworkConnection->Client();
            break;
        case ECommsType::ETypeNone:
            break;
    };

}

