/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef ENCRYPT_TLS__H
#define ENCRYPT_TLS__H


#include <string>
#include <memory>
#include <cstdint>

struct SEntryptILSData {
    std::string cert{""};
    std::string pkey{""};
};

namespace message { struct SMessage; }
struct SpImp;

class EncryptTLS {
public:
    EncryptTLS(const SEntryptILSData& parms);
    ~EncryptTLS();

    bool Accept(int sock);
    bool Connect(int sock);
    size_t Send(const message::SMessage& msg);
    size_t Receive(message::SMessage& msg);

private:    
    inline std::uint64_t htonll(std::uint64_t value);
    inline std::uint64_t ntohll(std::uint64_t value);
    std::unique_ptr<SpImp> mpData;
    SEntryptILSData mParms;
};

#endif // ENCRYPT_TLS__H
