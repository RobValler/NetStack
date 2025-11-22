/*****************************************************************
 * Copyright (C) 2017 - Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
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
#include <vector>
#include <iostream>

namespace {

    std::atomic<bool> gIsExitCalled = false;

    struct STestData {
        int num;
        std::string str;
    };
}

class CConnectTest {
public:

    CConnectTest(ECommsProto protocol)
        : mProtocol(protocol)
    { /**/ }

    ~CConnectTest() =default;

    void process() {

        TestMsgPackage message;
        SConnectParms parms;
        parms.portID = 1101;
        parms.ipAddress = "127.0.0.1";
        parms.proto = mProtocol;

        // ### SERVER ###
        auto threadServer = [&]() {

            parms.type = ECommsType::ETypeServer;
            CNetworkHndl network_server(parms);

            network_server.Start();

            while( !network_server.IsConnected() ){
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            for(const auto& it : send_data) {

                message.set_id(it.num);
                message.set_name(it.str);

                SNetIF local_data;
                local_data.name = "tom";
                network_server.Send(local_data, message);
                std::cout << "sent data" << std::endl;


                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            network_server.Stop();
        };

        // ### CLIENT ###
        auto threadClient = [&]() {

            parms.type = ECommsType::ETypeClient;
            CNetworkHndl network_client(parms);
            int index = 1;
            SNetIF local_data;
            STestData tmp_rec_data;

            network_client.Start();
            while( !gIsExitCalled ) {

                if(!network_client.IsConnected()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }

                local_data.name = "";
                if(network_client.Receive(local_data, message) > 0) {
                    std::cout << "received data" << std::endl;
                    tmp_rec_data.num = message.id();
                    tmp_rec_data.str = message.name();
                    rec_data.emplace_back(std::move(tmp_rec_data));
                }

                if(rec_data.size() == send_data.size()) {
                    break;
                }
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

    bool Compare() {

        bool result = true;

        if(send_data.size() == rec_data.size()) {
            for(const auto& it_send : send_data) {
                for(const auto& it_rec : rec_data) {
                    if( (it_send.num != it_rec.num) ||
                        (it_send.str != it_rec.str) )

                        result = false;
                }
            }
        } else {
            result = false;
        }
        std::cout << "compared data" << std::endl;
        return result;
    }

private:
    ECommsProto mProtocol;
    std::vector<STestData> rec_data;

    std::vector<STestData> send_data = {
        { 123, "cat" },
        { 456, "dog" },
        { 789, "cow" },
    };

};

TEST(connect, tcpip)
{
    CConnectTest test(ECommsProto::EProto_TCPIP);
    test.process();
    EXPECT_TRUE(test.Compare());
}

TEST(connect, udp)
{
    CConnectTest test(ECommsProto::EProto_UDP);
    test.process();
    EXPECT_TRUE(test.Compare());
}

TEST(connect, posix)
{
    CConnectTest test(ECommsProto::EProto_POSIX);
    test.process();
    EXPECT_TRUE(test.Compare());
}
