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
#include "udp_stack.h"
#include "ftp_hndl.h"
#include "file_hndl.h"

#include "testMsgPackage.pb.h"
#include "serialise.h"
#include "message_define.h"

#include <thread>
#include <chrono>
#include <vector>
#include <iostream>

template<class SERV, class CLIENT>
class TestClass
{
public:
    TestClass() =default;
    ~TestClass() =default;

    bool Run() {

        bool run_result = true;

        // ### TEST DATA ###
        struct STestData {
            int num;
            std::string str;
        };

        std::vector<STestData>
            send_data_to = {
                { 123, "cat" },
                { 456, "dog" },
                { 789, "cow" },
            };

        std::vector<STestData>
            send_data_from = {
                { 444, "meaow" },
                { 555, "woof" },
                { 777, "moo" },
            };

        CSerial serialise;

        // ### SERVER ###
        auto threadServer = [&]() {

            message::SMessage msg;
            SConnectParms parms_server;
            parms_server.portID = 8080;
            SERV network_server(parms_server);

            network_server.Start();

            // wait for connection
            auto end_time = std::chrono::system_clock::now() + std::chrono::seconds(5);
            while(true) {
                if(network_server.Connections() > 0) {
                    break;
                } else {
                    auto time_now = std::chrono::system_clock::now();
                    if(time_now > end_time) {
                        // connection failed, leave
                        std::cout << "Connection timeout, exiting" << std::endl;

                        run_result = false;
                        return;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }

            // SEND
            for(const auto& it : send_data_to) {

                TestMsgPackage send_message;
                send_message.set_msgid(it.num);
                send_message.set_msgname(it.str);

                int size;
                if(!serialise.Serialise(send_message, msg.mMsgPayload, size)) {
                    std::cerr << "error: Serialise" << std::endl;
                    continue;
                }

                //msg.ID = 10; ???
                if(network_server.Send(msg) <= 0) {
                    std::cerr << "server send error" << std::endl;
                }
            }

            // REC
            for(int index = 0; index < send_data_to.size(); ++index) {

                msg.mMsgPayload.clear();
                msg.mConnectionID = 10;
                if(network_server.Receive(msg) > 0) {

                    TestMsgPackage rec_message;
                    int size = msg.mMsgPayload.size();
                    if(!serialise.Deserialise(msg.mMsgPayload, rec_message, size)) {
                        std::cerr << "server serialise error" << std::endl;
                        continue;
                    }

                    auto id = rec_message.msgid();
                    auto name = rec_message.msgname();
                    std::cout << "Server: ID = " << id
                              << ", Name = " << name
                              << std::endl;
                } else {

                    std::cerr << "server receive error" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
            } //

            network_server.Stop();
        };

        // ### CLIENT ###
        auto threadClient = [&]() {

            message::SMessage msg;

            SConnectParms parms_client;
            parms_client.ipAddress = "127.0.0.3";
            parms_client.portID = 8080;

            CLIENT network_client(parms_client);

            if (0 != network_client.Start()) {

                std::cerr << "client start error" << std::endl;
                run_result = false;
                return;
            }

            // RECV
            for(int index = 0; index < send_data_to.size(); ++index) {

                msg.mMsgPayload.clear();
                if(network_client.Receive(msg) > 0) {

                    TestMsgPackage rec_message;
                    int size = msg.mMsgPayload.size();
                    if(!serialise.Deserialise(msg.mMsgPayload, rec_message, size)) {
                        std::cerr << "client serialise error" << std::endl;
                        continue;
                    }

                    auto id = rec_message.msgid();
                    auto name = rec_message.msgname();
                    std::cout << "Client: ID = " << id
                              << ", Name = " << name
                              << std::endl;
                } else {

                    std::cerr << "client receive error" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
            } //

            // SEND
            for(const auto& it : send_data_from) {

                TestMsgPackage send_message;
                send_message.set_msgid(it.num);
                send_message.set_msgname(it.str);

                int size;
                if(!serialise.Serialise(send_message, msg.mMsgPayload, size)) {
                    std::cerr << "client serialise err" << std::endl;
                    continue;
                }

                if(network_client.Send(msg) <= 0) {
                    std::cout << "client send error" << std::endl;
                }
            }

            network_client.Stop();
        };

        std::thread tServer(threadServer);
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        std::thread tClient(threadClient); // client is receiver, receiver creates the channel for MQ

        tClient.join();
        tServer.join();

        return run_result;
    } // Run()

private:


};

TEST(tcpip, basic)
{
    TestClass <CTCPIP_Server, CTCPIP_Client> t;
    EXPECT_EQ(t.Run(), true);
}

TEST(tcpip, multi)
{    
    SConnectParms parms_server;
    parms_server.ipAddress = "127.0.0.1";
    parms_server.portID = 8080;
    CTCPIP_Server network_server(parms_server);
    network_server.Start();

    std::vector<std::shared_ptr<CTCPIP_Client>> local_client_list;
    const int local_max_num_of_clients = 10;
    for(int local_client_index = 0; local_client_index <= local_max_num_of_clients; ++local_client_index) {

        auto foo = std::make_shared<CTCPIP_Client>(parms_server);
        foo->Start();
        local_client_list.push_back(std::move(foo));
        std::cout << "Number of clients = " << std::to_string(network_server.Connections()) << std::endl;
    }

    //auto moo = network_server.

    message::SMessage msg;
    network_server.Send(msg);


    network_server.Stop();
    std::cout << "Number of clients after Stop() = " << std::to_string(network_server.Connections()) << std::endl;
}


TEST(udp, basic)
{
    CSerial serialise;

    auto tlocal = [&]() {

        message::SMessage message;
        SConnectParms parms_local;
        parms_local.portLocalID = 9000;
        parms_local.portRemoteID = 9001;
        CUDP_Stack port_local(parms_local);

        port_local.Start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        TestMsgPackage send_message;
        send_message.set_msgid(1);
        send_message.set_msgname("local test");

        message::SMessage msg;

        int size;
        msg.mConnectionID = 5;
        serialise.Serialise(send_message, msg.mMsgPayload, size);
        port_local.Send(msg);

        // REC
        msg.mMsgPayload.clear();
        msg.mConnectionID = 10;
        if(port_local.Receive(msg) > 0) {

            TestMsgPackage rec_message;
            int size = msg.mMsgPayload.size();
            serialise.Deserialise(msg.mMsgPayload, rec_message, size);

            auto id = rec_message.msgid();
            auto name = rec_message.msgname();
            std::string str = "local message: (" + std::to_string(id) + ") " + rec_message.msgname()
                                  + " from ip address: " + msg.mIpAddress + "\n";
            std::cout << str;
        }

        port_local.Stop();
    };

    auto tremote = [&]() {

        SConnectParms parms_remote;
        parms_remote.portLocalID = 9001;
        parms_remote.portRemoteID = 9000;
        CUDP_Stack port_remote(parms_remote);


        port_remote.Start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // SEND
        TestMsgPackage send_message;
        send_message.set_msgid(2);
        send_message.set_msgname("remote test");

        message::SMessage msg;
        int size;
        msg.mConnectionID = 6;
        serialise.Serialise(send_message, msg.mMsgPayload, size);
        port_remote.Send(msg);

        // REC
        msg.mMsgPayload.clear();
        msg.mConnectionID = 10;
        if(port_remote.Receive(msg) > 0) {

            TestMsgPackage rec_message;
            int size = msg.mMsgPayload.size();
            serialise.Deserialise(msg.mMsgPayload, rec_message, size);

            auto id = rec_message.msgid();
            auto name = rec_message.msgname();
            std::string str = "remote message: (" + std::to_string(id) + ") " + rec_message.msgname() + "\n";
            std::cout << str;
        }

        port_remote.Stop();
    };

    std::thread tServer(tlocal);
    std::thread tClient(tremote);

    tClient.join();
    tServer.join();
}

TEST(posix_mq, basic)
{
    TestClass <CPOSIX_MQ_Node, CPOSIX_MQ_Node> t;
    EXPECT_EQ(t.Run(), true);
}

TEST(sftp, basic)
{
    PutCommand("/home/rob/WORK/C_CPP/NetStack/build/Debug/moo.txt");
}

TEST(file, basic)
{
    SXmlData data;
    XMLReadconst("/home/rob/WORK/C_CPP/NetStack/test/data/test.xml", data);
}
