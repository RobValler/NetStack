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

class CEncrypt
{
public:
    CEncrypt() =default;
    ~CEncrypt() =default;

    bool Encrypt(const std::vector<char>& input_data, std::vector<char>& output_data);
    bool Decrypt(const std::vector<char>& input_data, std::vector<char>& output_data);

};

#endif // ENCRYPT__H
