/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_server_conn.h"

#include "message_define.h"
#include "encrypt_tls.h"
#include "logger.h"

#include <unistd.h>
#ifdef __linux__
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

CTCPIP_ClientConn::CTCPIP_ClientConn(SClientEntryCont parms)
    : mParms(parms)
    , mpTLS(std::make_shared<EncryptTLS>(SEntryptILSData{parms.cert, parms.pkey}))
{ /**/ }

CTCPIP_ClientConn::~CTCPIP_ClientConn()
{ /**/ }

bool CTCPIP_ClientConn::Start() {

    if(!mpTLS->Accept(mParms.mClientFD)) {
        CLogger::Err("[TPIP Server Con] TLS connect error");
        return false;
    }
    return true;
}

void CTCPIP_ClientConn::Stop() {

    close(mParms.mClientFD);
}

int CTCPIP_ClientConn::Send(const message::SMessage& msg_data) {

    if(!mConnected) {
        return 0;
    }
    return mpTLS->Send(msg_data);
}

int CTCPIP_ClientConn::Receive(message::SMessage& msg_data) {

    if(!mConnected) {
        return 0;
    }
    return mpTLS->Receive(msg_data);
}

bool CTCPIP_ClientConn::Connected() {

    return mConnected;
};
