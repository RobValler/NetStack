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
#include "testMsgPackage.pb.h"
#include "serialise.h"

#include <thread>
#include <chrono>

#include <vector>
#include <iostream>

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
        std::vector<std::uint8_t> stream_data;
        int stream_size;
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

            stream_size = 0;
            stream_data.clear();

            ser.Serialise(send_message, stream_data, stream_size);
            if(network_server.Send(data, stream_data) <= 0) {
                std::cout << "send error" << std::endl;
            }
        }

        network_server.Stop();
    };

    auto threadClient = [&]() {

        // ### CLIENT ###
        std::vector<std::uint8_t> stream_data;
        int stream_size;
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

            stream_size = 0;
            stream_data.clear();

            if(network_client.Receive(data, stream_data) > 0) {

                TestMsgPackage rec_message;
                ser.Deserialise(stream_data, rec_message, stream_size);

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
