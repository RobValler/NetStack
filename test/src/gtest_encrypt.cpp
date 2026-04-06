/*****************************************************************
 * Copyright (C) 2017 - Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

#include "encrypt_aes.h"
#include "encrypt_tls.h"

#include "tcpip_server.h"
#include "tcpip_client.h"

#include "message_define.h"
#include "testMsgPackage.pb.h"
#include "serialise.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

#include <cstdint>
#include <iostream>

TEST(encrypt, AES)
{
    std::uint8_t local_key[32];
    std::uint8_t local_iv[16];

    // Generate random key and IV
    Rand(local_key, sizeof(local_key));
    Rand(local_iv, sizeof(local_iv));

    std::string input = "ThIs Is A tEsT!";
    std::vector<std::uint8_t> raw_text(input.begin(), input.end());
    std::vector<std::uint8_t> encrypted_text;
    std::vector<std::uint8_t> decrypted_text;
    std::string output = "";


    EXPECT_TRUE(EncryptAES(raw_text, local_key, local_iv, encrypted_text));

    output.assign(encrypted_text.begin(), encrypted_text.end());
    std::cout << "Intermediate = " << output << std::endl; // lets see some gibberish

    EXPECT_TRUE(DecryptAES(encrypted_text, local_key, local_iv, decrypted_text));

    output.clear();
    output.assign(decrypted_text.begin(), decrypted_text.end());
    std::cout << "Result = " << output << std::endl;
}

TEST(encrypt, TLS) {

    std::atomic<bool> ExitCalled = false;

    // ### SERVER ###
    auto threadServerTCPIP = [&]() {

        TestMsgPackage test_msg;
        message::SMessage msg;
        CSerial serialise;


        CTCPIP_Server tcpip_server;
        STCPIPServParms parms;
        parms.portID = 2001;
        tcpip_server.Start(parms);
        while(!ExitCalled) {

            if(tcpip_server.Receive(msg) > 0) {

                int size = msg.mMsgPayload.size();
                if(!serialise.Deserialise(msg.mMsgPayload, test_msg, size)) {
                    std::cerr << "error: Deserialise" << std::endl;
                    continue;
                }

                std::cout << "Name = " << test_msg.msgname() << ", ID = " << test_msg.msgid() << std::endl;

            } else {
                //std::cerr << " thread server rec error" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        tcpip_server.Stop();
    };

    // ### CLIENT ###
    auto threadClientTCPIP = [&]() {

        TestMsgPackage test_msg;
        message::SMessage msg;

        CSerial serialise;
        CTCPIP_Client tcpip_client;
        STCPIPClientParms tcpip_parms;
        tcpip_parms.portID = 2001;
        tcpip_parms.localIpAddress = "127.0.0.1";
        tcpip_parms.remoteIpAddress = "127.0.0.1";
        tcpip_parms.maxConnectRetryAttempts = 10;
        if(1 == tcpip_client.Start(tcpip_parms)) {
            std::cerr << "error: tcpip_client start failed" << std::endl;
        }

        int index = 0;
        while(!ExitCalled) {

            if(!tcpip_client.Connection()) {

                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            test_msg.set_msgid(123);
            test_msg.set_msgname("Test " + std::to_string(index++));
            int size;
            if(serialise.Serialise(test_msg, msg.mMsgPayload, size)) {

                if(tcpip_client.Send(msg) <= 0) {
                    std::cerr << "error: tcpip send failed" << std::endl;
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        tcpip_client.Stop();
    };

    std::thread tServerTCPIP(threadServerTCPIP);
    std::thread tClientTCPIP(threadClientTCPIP);


    //ExitCalled = true;


    tClientTCPIP.join();
    tServerTCPIP.join();
}
