/*****************************************************************
 * Copyright (C) 2017 - Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "encrypt.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <iostream>

TEST(encrypt, example)
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

