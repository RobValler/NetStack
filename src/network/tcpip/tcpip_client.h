/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef TCPIP_CLIENT__H
#define TCPIP_CLIENT__H

#include "network_connect_parms.h"

#include "tcpip_helper.h"

namespace message { struct SMessage; }

class CTCPIP_Client : public CTCPIP_Helper
{
public:
    CTCPIP_Client(const SConnectParms& parms);
    ~CTCPIP_Client() =default;

    int Start() override;
    void Stop() override;

private:
    SConnectParms mConnectParms;

};

#endif // TCPIP_CLIENT__H
