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



#include <memory>

namespace message { struct SMessage; }
struct SpImp;

class EncryptTLS {
public:
    EncryptTLS();
    ~EncryptTLS();

    bool Accept(int sock);
    bool Connect(int sock);
    int Send(const message::SMessage& msg);
    int Receive(message::SMessage& msg);

private:
    std::unique_ptr<SpImp> mpData;
};

#endif // ENCRYPT_TLS__H
