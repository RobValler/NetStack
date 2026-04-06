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

// OpenSSL AES
bool EncryptAES(const std::vector<std::uint8_t>& plaintext,
            const std::uint8_t* key,
            const std::uint8_t* iv,
            std::vector<std::uint8_t>& ciphertext);

bool DecryptAES(const std::vector<std::uint8_t>& ciphertext,
            const std::uint8_t* key,
            const std::uint8_t* iv,
            std::vector<std::uint8_t>& plaintext);

// OpenSSL TLS
bool EncryptTLS();

bool DecryptTLS();

void Rand(std::uint8_t* key, int size);

#endif // ENCRYPT__H
