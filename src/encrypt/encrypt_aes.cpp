/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "encrypt_aes.h"

#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

bool EncryptAES(const std::vector<std::uint8_t>& raw_text,
             const std::uint8_t* key,
             const std::uint8_t* init_vect,
             std::vector<std::uint8_t>& encrypted_text) {

    int local_len;
    int encrypted_text_len = 0;

    EVP_CIPHER_CTX* local_context = EVP_CIPHER_CTX_new();
    if (!local_context) {
        return false;
    }

    encrypted_text.resize(raw_text.size() + EVP_MAX_BLOCK_LENGTH);

    if (1 != EVP_EncryptInit_ex(local_context, EVP_aes_256_cbc(), NULL, key, init_vect)) {
        return false;
    }

    if (1 != EVP_EncryptUpdate(local_context, encrypted_text.data(), &local_len, raw_text.data(), raw_text.size())) {
        return false;
    }

    encrypted_text_len = local_len;

    if (1 != EVP_EncryptFinal_ex(local_context, encrypted_text.data() + local_len, &local_len)) {
        return false;
    }

    encrypted_text_len += local_len;
    encrypted_text.resize(encrypted_text_len);

    EVP_CIPHER_CTX_free(local_context);

    return true;
}

bool DecryptAES(const std::vector<std::uint8_t>& encrypted_text,
             const std::uint8_t* key,
             const std::uint8_t* init_vect,
             std::vector<std::uint8_t>& raw_text) {

    int local_len;
    int raw_text_len;

    EVP_CIPHER_CTX* local_context = EVP_CIPHER_CTX_new();
    if (!local_context) {
        return false;
    }

    raw_text.resize(encrypted_text.size());

    if (1 != EVP_DecryptInit_ex(local_context, EVP_aes_256_cbc(), NULL, key, init_vect)) {
        return false;
    }

    if (1 != EVP_DecryptUpdate(local_context, raw_text.data(), &local_len, encrypted_text.data(), encrypted_text.size())) {
        return false;
    }

    raw_text_len = local_len;

    if (1 != EVP_DecryptFinal_ex(local_context, raw_text.data() + local_len, &local_len)) {
        return false;
    }

    raw_text_len += local_len;
    raw_text.resize(raw_text_len);

    EVP_CIPHER_CTX_free(local_context);

    return true;
}

void Rand(std::uint8_t* key, int size){

    RAND_bytes(key, size);
}


