/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef NETWORK_CONNECT_PARMS__H
#define NETWORK_CONNECT_PARMS__H

#include <string>

enum class ECommsType: int {
    ETypeNone = 0,
    ETypeServer,
    ETypeClient,
};

enum class ECommsProto: int {
    EProto_None = 0,
    EProto_TCPIP,
    EProto_UDP,
    EProto_POSIX
};

struct SConnectParms {

    std::string name;
    ECommsType type{ECommsType::ETypeNone};
    ECommsProto proto{ECommsProto::EProto_None};
    int portID;
    std::string ipAddress;
};

struct SNetIF {
    std::string name;
};

#endif // NETWORK_CONNECT_PARMS__H
