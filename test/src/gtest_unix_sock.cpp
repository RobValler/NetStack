/*****************************************************************
 * Copyright (C) 2017 - Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

#include "uds_ipc.h"
#include "logger.h"


TEST(network, unix_sock)
{
    CUDSIPC client;
    if(client.Start("/tmp/uds_test.sock")) {

        std::string msg1 = "We are the knights who say .... UNHANDLED EXCEPTION!!";
        client.Send(msg1);

        std::string msg2 = "";
        client.Receive(msg2);
        CLogger::Print("Received message = " + msg2);

    } else {
        CLogger::Print("error");
        FAIL();
    }

    client.Stop();
}
