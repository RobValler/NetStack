/*****************************************************************
 * Copyright (C) 2017 - Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

#include "network_hndl.h"

#include "testMsgPackage.pb.h"

#include "network_connect_parms.h"

#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>

namespace {
    std::atomic<bool> gIsExitCalled = false;
}

TEST(tcpip, connection)
{
    SNetIF local_data;

    // ### SERVER ###
    auto threadServer = [&]() {

        TestMsgPackage message;
        SConnectParms parms;
        parms.portID = 1101;
        parms.ipAddress = "127.0.0.1";
        parms.type = ECommsType::ETypeServer;
        CNetworkHndl network_server(parms);
        int index = 0;

        network_server.Start();
        while( !gIsExitCalled ) {

            message.set_id(index++);
            message.set_name("data");

            SNetIF local_data;
            local_data.name = "tom";
            network_server.Send(local_data, message);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        network_server.Stop();
    };

    // ### CLIENT ###
    auto threadClient = [&]() {

        TestMsgPackage message;
        SConnectParms parms;
        parms.portID = 1101;
        parms.ipAddress = "127.0.0.1";
        parms.type = ECommsType::ETypeClient;
        CNetworkHndl network_client(parms);
        int index = 1;

        network_client.Start();
        while( !gIsExitCalled ) {

            if(!network_client.IsConnected()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            local_data.name = "";
            if(network_client.Receive(local_data, message) > 0) {
                index = message.id();
                std::cout << "message id = " << std::to_string(index)
                          << ", name = " << message.name()
                          << std::endl;
            }

            if(index >= 5) {
                gIsExitCalled = true;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        network_client.Stop();
    };

    std::thread tServer(threadServer);
    std::thread tClient(threadClient);

    tClient.join();
    tServer.join();

    std::cout << "Test ended" << std::endl;
    std::cout.flush();
}
