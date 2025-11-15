/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef ENCRYPT__H
#define ENCRYPT__H

#include <vector>
#include <cstdint>

// struct SSL_CTX;
// struct SSL;

class CEncrypt
{
public:
    CEncrypt();
    ~CEncrypt();

    bool Encrypt(const std::vector<std::uint8_t>& input_data, std::vector<std::uint8_t>& output_data);
    bool Decrypt(const std::vector<std::uint8_t>& input_data, std::vector<std::uint8_t>& output_data);

private:
    // SSL_CTX *ctx;
    // SSL *ssl;

};

#endif // ENCRYPT__H
