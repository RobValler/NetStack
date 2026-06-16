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
#include "json_to_struct.h"
#include "logger.h"

TEST(unix_sock, basic)
{
    CUDSIPC client;

}

TEST(unix_sock, get)
{
    CUDSIPC client;
    std::string msg = "We are the knights who say .... UNHANDLED EXCEPTION!!";
    client.Get("/tmp/uds_test.sock", msg);
    CLogger::Print("Sent message = " + msg);
}

TEST(unix_sock, json_test)
{
    CUDSIPC client;
    CJsonToStruct parser;
    std::string msg = "list";
    client.Get("/tmp/uds_test.sock", msg);

    std::vector<SDeviceData> local_dev = parser.Parse(msg);

    for(const auto& it : local_dev) {

        CLogger::Print("device_name = " + it.device_name);
        CLogger::Print("mount_point = " + it.mount_point);
        CLogger::Print("other = " + it.other);
    }
}
