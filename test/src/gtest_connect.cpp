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
#include "tcpip_client.h"

#include "message_define.h"
#include "testMsgPackage.pb.h"
#include "serialise.h"
#include "logger.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

TEST(network, connect)
{
    std::atomic<bool> ExitCalled = false;
    std::string tcpipServerIP = "";

    // ### SERVER ###
    auto threadServerUDP = [&]() {

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
        //udp_parms.localIpAddress = "192.168.100.11";

        udp_parms.broadCastSender = true;
        udp_parms.remoteIpAddress = "192.168.100.255";
        // udp_parms.broadCastSender = false;
        // udp_parms.remoteIpAddress = "192.168.100.12";
        udp_stack.Start(udp_parms);

        while(!ExitCalled) {

            int size;
            if(!serialise.Serialise(send_message, msg.mMsgPayload, size)) {

                std::cerr << "error: Serialise" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                continue;
            }

            if(0< udp_stack.Send(msg)) {

                std::cout << "Send OK" << std::endl;
            } else {

                std::cerr << "error: Send" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        udp_stack.Stop();
    };


    auto threadServerTCPIP = [&]() {

        CTCPIP_Server tcpip_server;
        STCPIPServParms parms;
        parms.portID = 2001;
        parms.cert = "../cert/cert.pem";
        parms.pkey = "../cert/key.pem";
        tcpip_server.Start(parms);
        while(!ExitCalled) {


            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        tcpip_server.Stop();
    };

    // ### CLIENT ###
    auto threadClientUDP = [&]() {

        CUDP_Stack mUDPStack;
        message::SMessage msg;
        CSerial serialiser;
        TestMsgPackage rec_message;

        // Start the UDP
        SUDPParms udp_parms;
        //udp_parms.broadCastSender = false;
        udp_parms.portLocalID = 8002;
        udp_parms.portRemoteID = 8001;
        //udp_parms.localIpAddress = "192.168.100.12";
        udp_parms.remoteIpAddress = "0.0.0.0"; //"192.168.100.12";
        mUDPStack.Start(udp_parms);

        while(!ExitCalled) {

            if(mUDPStack.Receive(msg) <= 0) {
                std::cerr << "error: Receive" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            } else {
                CLogger::Print(std::string("UDP message received"));
            }

            int size = msg.mMsgPayload.size();
            if(!serialiser.Deserialise(msg.mMsgPayload, rec_message, size)) {
                std::cerr << "error: Deserialise" << std::endl;
                continue;
            }

#if 1
            std::cout << "Client : received data from ("
                      << msg.mIpAddress
                      << ":"
                      << std::to_string(msg.mPort)
                      << ") - "
                      << rec_message.msgid()
                      << ", "
                      << rec_message.msgname() << std::endl;
#endif

            tcpipServerIP = msg.mIpAddress;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        mUDPStack.Stop();
    };

    auto threadClientTCPIP = [&]() {

        while(!ExitCalled) {

            if("" == tcpipServerIP) {

                // not ready yet
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            } else {
                break;
            }
        }

        CTCPIP_Client tcpip_client;
        STCPIPClientParms tcpip_parms;
        tcpip_parms.portID = 2001;
        tcpip_parms.localIpAddress = "192.168.100.12";
        tcpip_parms.remoteIpAddress = tcpipServerIP;
        tcpip_parms.maxConnectRetryAttempts = 10;
        tcpip_parms.cert = "../cert/cert.pem";
        tcpip_parms.pkey = "../cert/key.pem";        
        if(1 == tcpip_client.Start(tcpip_parms)) {
            std::cerr << "error: tcpip_client start failed" << std::endl;
        }

        while(!ExitCalled) {

            std::this_thread::sleep_for(std::chrono::seconds(2));
        }

        tcpip_client.Stop();

    };

    std::thread tServerUDP(threadServerUDP);
    std::thread tServerTCPIP(threadServerTCPIP);
    std::thread tClientUDP(threadClientUDP);
    std::thread tClientTCPIP(threadClientTCPIP);

    std::cout << "Press Key to continue..."  << std::endl;;
    std::cout << "A key WAS pressed :D"  << std::endl;;

    //ExitCalled = true;

    tClientUDP.join();
    tClientTCPIP.join();
    tServerUDP.join();
    tServerTCPIP.join();


}

