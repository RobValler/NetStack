/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "encrypt.h"

#include <openssl/ssl.h>
#include <openssl/err.h>

CEncrypt::CEncrypt()
{
    // SSL_library_init();
    // SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    // SSL *ssl = SSL_new(ctx);
}

CEncrypt::~CEncrypt() {

    // SSL_CTX_free(ctx);
}

bool CEncrypt::Encrypt(const std::vector<std::uint8_t>& input_data, std::vector<std::uint8_t>& output_data) {

    output_data = input_data;
    return true;
}

bool CEncrypt::Decrypt(const std::vector<std::uint8_t>& input_data, std::vector<std::uint8_t>& output_data) {

    output_data = input_data;
    return true;
}
