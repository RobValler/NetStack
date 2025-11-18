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

#include "network_connect_parms.h"
#include "serialise.h"
#include "encrypt.h"

#include "network_tcpip.h"
#include "network_udp.h"

#include <google/protobuf/message.h>

#include <iostream>

CNetworkHndl::CNetworkHndl(const SConnectParms& parms)
    : mParms(parms)    
    , mpSerialiser(std::make_unique<CSerial>())
    , mpEncrypt(std::make_unique<CEncrypt>())
{ /* nothing */ }

CNetworkHndl::~CNetworkHndl()
{ /* nothing */ }

void CNetworkHndl::Start() {

    // blocking call
    mtServer = std::thread(&CNetworkHndl::ThreadFuncServer, this);
}

void CNetworkHndl::Stop() {

    mpNetworkConnection->Stop();
    mtServer.join();

    std::string local_type_str;
    switch(mParms.type) {
        case ECommsType::ETypeNone: local_type_str = "unknown"; break;
        case ECommsType::ETypeServer: local_type_str = "server"; break;
        case ECommsType::ETypeClient: local_type_str = "client"; break;
            break;
    }

    std::cout << local_type_str << " stopped " << std::endl;
}

bool CNetworkHndl::IsConnected() {

    if(mpNetworkConnection) {
        mIsThereAConnection = mpNetworkConnection->IsConnected();
    } else {
        mIsThereAConnection = false;
    }
    return mIsThereAConnection;
}

int CNetworkHndl::Receive(const SNetIF& operater, google::protobuf::Message& proto_message) {

    if(!mIsInitialised) {
        return 0;
    }

    // read the data from the network
    std::vector<std::uint8_t> incomming_data;
    int incomming_data_size = mpNetworkConnection->Receive(operater, incomming_data); // blocking
    if(0 == incomming_data_size) {
        return 0;
    }
    if(0 > incomming_data_size) {
        return 0;
    }

    // decrypt the data
    std::vector<std::uint8_t> decrypted_data;
    if(!mpEncrypt->Decrypt(incomming_data, decrypted_data)) {
        return 0;
    }

    // deserialise the data from vector array to protobuf formatted message class
    mpSerialiser->Deserialise(decrypted_data, proto_message, incomming_data_size);

    return incomming_data_size;
}

int CNetworkHndl::Send(const SNetIF& operater, const google::protobuf::Message& proto_message) {

    if(!mIsInitialised) {
        return 0;
    }

    // check connection
    if((!IsConnected()) &&
        (ECommsProto::EProto_UDP != mParms.proto) ) {
        return 0;
    }

    // serialise the data from the protobuf formatted message class to a vector array
    std::vector<std::uint8_t> serialised_data;
    int serialised_data_size = 0;
    if(!mpSerialiser->Serialise(proto_message, serialised_data, serialised_data_size)) {
        return 0;
    }

    // encrypt the data
    std::vector<std::uint8_t>encrypted_data;
    if(!mpEncrypt->Encrypt(serialised_data, encrypted_data)) {
        return 0;
    }

    // send the message
    int result = mpNetworkConnection->Send(operater, encrypted_data);

    return result;
}

void CNetworkHndl::ThreadFuncServer() {

    // select the protocol
    switch(mParms.proto) {
        case ECommsProto::EProto_None:
            return;
        case ECommsProto::EProto_TCPIP:
            mpNetworkConnection = std::make_unique<CNetworkTCPIP>(mParms);
            break;
        case ECommsProto::EProto_UDP:
            mpNetworkConnection = std::make_unique<CNetwork_UDP>(mParms);
            break;
        case ECommsProto::EProto_POSIX:
            //mpNetworkConnection = std::make_unique<CNetworkTCPIP>(mParms);
            //break;
            return;
    }

    mIsInitialised = true;

    switch(mParms.type)
    {
        case ECommsType::ETypeServer:
            mpNetworkConnection->Server();
            break;
        case ECommsType::ETypeClient:
            mpNetworkConnection->Client();
            break;
        case ECommsType::ETypeNone:
            break;
    };
}
