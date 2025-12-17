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

#include "testMsgPackage.pb.h"
#include "serialise.h"

#include "message_define.h"

#include <thread>
#include <chrono>
#include <vector>
#include <iostream>



TEST(tcpip, basic)
{
    // ### TEST DATA ###
    struct STestData {
        int num;
        std::string str;
    };

    std::vector<STestData> send_data = {
                                        { 123, "cat" },
                                        { 456, "dog" },
                                        { 789, "cow" },
                                        };


    std::vector<STestData> send_data_2 = {
                                        { 444, "meaow" },
                                        { 555, "woof" },
                                        { 777, "moo" },
                                        };


    CSerial ser;

    auto threadServer = [&]() {

        // ### SERVER ###
        message::SMessage msg;
        SConnectParms parms_server;
        parms_server.ipAddress = "127.0.0.1";
        parms_server.portID = 8080;
        CTCPIP_Server network_server(parms_server);

        if (0 != network_server.Start()) {

            std::cout << "TCPIP::Start error" << std::endl;
            return;
        }

        // SEND
        for(const auto& it : send_data) {
            //while(true) {
            TestMsgPackage send_message;
            send_message.set_msgid(it.num);
            send_message.set_msgname(it.str);

            //msg.body.data_array.clear();
            int size;
            if(!ser.Serialise(send_message, msg.data_array, size)) {
                std::cerr << "error: Serialise" << std::endl;
                continue;
            }
            //msg.body_size = size;

            if(network_server.Send(msg) <= 0) {
                std::cout << "send error" << std::endl;
            }
        }

        // REC
        for(int index = 0; index < send_data.size(); ++index) {

            msg.data_array.clear();
            if(network_server.Receive(msg) > 0) {

                TestMsgPackage rec_message;
                int size = msg.data_array.size();
                if(!ser.Deserialise(msg.data_array, rec_message, size)) {
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

    auto threadClient = [&]() {

        // ### CLIENT ###
        message::SMessage msg;

        SConnectParms parms_client;
        parms_client.ipAddress = "127.0.0.1";
        parms_client.portID = 8080;
        CTCPIP_Client network_client(parms_client);

        if (0 != network_client.Start()) {

            std::cout << "TCPIP::Start error" << std::endl;
            return;
        }

        // RECV
        for(int index = 0; index < send_data.size(); ++index) {

            msg.data_array.clear();
            if(network_client.Receive(msg) > 0) {

                TestMsgPackage rec_message;
                int size = msg.data_array.size();
                if(!ser.Deserialise(msg.data_array, rec_message, size)) {
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
        for(const auto& it : send_data_2) {
            //while(true) {
            TestMsgPackage send_message;
            send_message.set_msgid(it.num);
            send_message.set_msgname(it.str);

            //msg.body.data_array.clear();
            int size;
            if(!ser.Serialise(send_message, msg.data_array, size)) {
                std::cerr << "error: Serialise" << std::endl;
                continue;
            }
            //msg.body_size = size;

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

#if 0
TEST(posix_mq, basic)
{
    // ### TEST DATA ###
    struct STestData {
        int num;
        std::string str;
    };

    std::vector<STestData> send_data = {
        { 123, "cat" },
        { 456, "dog" },
        { 789, "cow" },
    };

    auto threadServer = [&]() {

        // ### SERVER ###
        message::SMessage msg;
        CSerial ser;

        SConnectParms parms_server;
        parms_server.channel_send = "/to_client";
        parms_server.channel_recv = "/to_server";
        CPOSIX_MQ_Node network_server(parms_server);
        if (0 != network_server.Start()) {

            std::cout << "CPOSIX_MQ_Server::Start error" << std::endl;
            return;
        }

        for(const auto& it : send_data) {
        //while(true) {
            TestMsgPackage send_message;
            send_message.set_id(it.num);
            send_message.set_name(it.str);
            SNetIF data;
            data.name = "output_tester";

            msg.body.text.clear();
            int size;
            //ser.Serialise(send_message, msg.body.data_array, size);

            if(network_server.Send(msg) <= 0) {
                std::cout << "send error" << std::endl;
            }
        }

        network_server.Stop();
    };

    auto threadClient = [&]() {

        // ### CLIENT ###
        message::SMessage msg;
        CSerial ser;

        SConnectParms parms_client;
        parms_client.channel_send = "/to_server";
        parms_client.channel_recv = "/to_client";
        CPOSIX_MQ_Node network_client(parms_client);
        if (0 != network_client.Start()) {

            std::cout << "CPOSIX_MQ_Client::Start error" << std::endl;
            return;
        }

        for(int index = 0; index < send_data.size(); ++index) {

            SNetIF data;
            data.name = "input_tester";
            msg.body.data_array.clear();

            if(network_client.Receive(msg) > 0) {

                TestMsgPackage rec_message;
                int size = msg.body.data_array.size();
                ser.Deserialise(msg.body.data_array, rec_message, size);

                std::cout << "ID = " << rec_message.id()
                          << ", Name = " << rec_message.name()
                          << std::endl;
            } else {

                std::cout << "Receive error" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
        } //
        network_client.Stop();
    };

    std::thread tClient(threadClient); // client is receiver, receiver creates the channel for MQ
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    std::thread tServer(threadServer);

    tClient.join();
    tServer.join();

}
#endif
