/*****************************************************************
 * Copyright (C) 2017 - Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

#include "posix_mq_node.h"
#include "tcpip_server.h"
#include "tcpip_client.h"
#include "network_udp.h"

#include "testMsgPackage.pb.h"
#include "serialise.h"
#include "message_define.h"

#include <thread>
#include <chrono>
#include <vector>
#include <iostream>


template<class C, class D>
class TestClass
{
public:
    TestClass() {

    }
    ~TestClass() =default;

    bool Run() {

        // ### TEST DATA ###
        struct STestData {
            int num;
            std::string str;
        };

        std::vector<STestData> send_data_to = {
                                               { 123, "cat" },
                                               { 456, "dog" },
                                               { 789, "cow" },
                                               };

        std::vector<STestData> send_data_from = {
                                                 { 444, "meaow" },
                                                 { 555, "woof" },
                                                 { 777, "moo" },
                                                 };

        CSerial serialise;


        // ### SERVER ###
        auto threadServer = [&]() {

            message::SMessage msg;
            SConnectParms parms_server;
            parms_server.ipAddress = "127.0.0.1";
            parms_server.portID = 8080;
            parms_server.channel_send = "/to_client";
            parms_server.channel_recv = "/to_server";
            C network_server(parms_server);

            if (0 != network_server.Start()) {

                std::cout << "Start error" << std::endl;
                return;
            }

            // SEND
            for(const auto& it : send_data_to) {
                //while(true) {
                TestMsgPackage send_message;
                send_message.set_msgid(it.num);
                send_message.set_msgname(it.str);

                int size;
                if(!serialise.Serialise(send_message, msg.data_array, size)) {
                    std::cerr << "error: Serialise" << std::endl;
                    continue;
                }

                if(network_server.Send(msg) <= 0) {
                    std::cout << "send error" << std::endl;
                }
            }

            // REC
            for(int index = 0; index < send_data_to.size(); ++index) {

                msg.data_array.clear();
                if(network_server.Receive(msg) > 0) {

                    TestMsgPackage rec_message;
                    int size = msg.data_array.size();
                    if(!serialise.Deserialise(msg.data_array, rec_message, size)) {
                        std::cerr << "error: Serialise" << std::endl;
                        continue;
                    }

                    auto id = rec_message.msgid();
                    auto name = rec_message.msgname();
                    std::cout << "Server: ID = " << id
                              << ", Name = " << name
                              << std::endl;
                } else {

                    std::cout << "Receive error" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
            } //

            network_server.Stop();
        };

        // ### CLIENT ###
        auto threadClient = [&]() {

            message::SMessage msg;

            SConnectParms parms_client;
            parms_client.ipAddress = "127.0.0.1";
            parms_client.portID = 8080;
            parms_client.channel_send = "/to_server";
            parms_client.channel_recv = "/to_client";
            D network_client(parms_client);

            if (0 != network_client.Start()) {

                std::cout << "Start error" << std::endl;
                return;
            }

            // RECV
            for(int index = 0; index < send_data_to.size(); ++index) {

                msg.data_array.clear();
                if(network_client.Receive(msg) > 0) {

                    TestMsgPackage rec_message;
                    int size = msg.data_array.size();
                    if(!serialise.Deserialise(msg.data_array, rec_message, size)) {
                        std::cerr << "error: Serialise" << std::endl;
                        continue;
                    }

                    auto id = rec_message.msgid();
                    auto name = rec_message.msgname();
                    std::cout << "Client: ID = " << id
                              << ", Name = " << name
                              << std::endl;
                } else {

                    std::cout << "Receive error" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
            } //

            // SEND
            for(const auto& it : send_data_from) {

                TestMsgPackage send_message;
                send_message.set_msgid(it.num);
                send_message.set_msgname(it.str);

                int size;
                if(!serialise.Serialise(send_message, msg.data_array, size)) {
                    std::cerr << "error: Serialise" << std::endl;
                    continue;
                }

                if(network_client.Send(msg) <= 0) {
                    std::cout << "send error" << std::endl;
                }
            }

            network_client.Stop();
        };

        std::thread tServer(threadServer);
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        std::thread tClient(threadClient); // client is receiver, receiver creates the channel for MQ

        tClient.join();
        tServer.join();

    }

private:


};

TEST(tcpip, basic)
{
    TestClass <CTCPIP_Server, CTCPIP_Client> t;
    EXPECT_EQ(t.Run(), true);
}

TEST(posix_mq, basic)
{
    TestClass <CPOSIX_MQ_Node, CPOSIX_MQ_Node> t;
    EXPECT_EQ(t.Run(), true);
}

TEST(udp, basic)
{
    TestClass <CNetwork_UDP, CNetwork_UDP> t;
    EXPECT_EQ(t.Run(), true);
}
