/*****************************************************************
 * Copyright (C) 2017 - Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

#include "udp_stack.h"
#include "tcpip_server.h"
#include "message_define.h"
#include "testMsgPackage.pb.h"
#include "serialise.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

TEST(network, connect)
{
    std::atomic<bool> ExitCalled = false;

    // ### SERVER ###
    auto threadServer = [&]() {

        CUDP_Stack udp_stack;
        CSerial serialise;
        TestMsgPackage send_message;
        send_message.set_msgid(10);
        send_message.set_msgname("Discovery request");
        message::SMessage msg;

        // Start the UDP
        SUDPParms udp_parms;
        udp_parms.portLocalID = 8001;
        udp_parms.portRemoteID = 8002;
        udp_parms.broadCastSender = false;
        //udp_parms.localIpAddress = "192.168.100.11";
        udp_parms.remoteIpAddress = "192.168.100.11";
        udp_stack.Start(udp_parms);

        while(!ExitCalled) {

            int size;
            if(!serialise.Serialise(send_message, msg.mMsgPayload, size)) {

                std::cerr << "error: Serialise" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                continue;
            }

            if(0< udp_stack.Send(msg)) {


            } else {

                std::cerr << "error: Send" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };

    // ### CLIENT ###
    auto threadClient = [&]() {

        CUDP_Stack mUDPStack;
        message::SMessage msg;
        CSerial serialiser;
        TestMsgPackage rec_message;

        // Start the UDP
        SUDPParms udp_parms;
        udp_parms.broadCastSender = false;
        udp_parms.portLocalID = 8002;
        udp_parms.portRemoteID = 8001;
        //udp_parms.localIpAddress = "192.168.100.12";
        udp_parms.remoteIpAddress = "192.168.100.12";
        mUDPStack.Start(udp_parms);

        while(!ExitCalled) {

            if(0 >= mUDPStack.Receive(msg)) {
                std::cerr << "error: Receive" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            }

            int size = msg.mMsgPayload.size();
            if(!serialiser.Deserialise(msg.mMsgPayload, rec_message, size)) {
                std::cerr << "error: Deserialise" << std::endl;
                continue;
            }

            std::cout << "Client : received data from ("
                      << msg.mIpAddress
                      << ":"
                      << std::to_string(msg.mPort)
                      << ") - "
                      << rec_message.msgid()
                      << ", "
                      << rec_message.msgname() << std::endl;


            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };

    std::thread tServer(threadServer);
    std::thread tClient(threadClient); // client is receiver, receiver creates the channel for MQ

    std::cout << "Press Key to continue..."  << std::endl;;
    std::cout << "A key WAS pressed :D"  << std::endl;;

    //ExitCalled = true;

    tClient.join();
    tServer.join();
}

