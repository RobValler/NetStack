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

    std::string msg1 = "We are the knights who say .... UNHANDLED EXCEPTION!!";
    EXPECT_GT(client.Send("/tmp/uds_test.sock", msg1), 0);
    CLogger::Print("Sent message = " + msg1);

    std::string msg2 = "";
    EXPECT_GT(client.Receive("/tmp/uds_test.sock", msg2), 0);
    EXPECT_EQ("Here is your shrubbery!", msg2);
    CLogger::Print("Received message = " + msg2);
}
