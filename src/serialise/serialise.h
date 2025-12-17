/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef SERIALISE__H
#define SERIALISE__H

#include <vector>
#include <cstdint>

namespace google::protobuf { class Message; }

class CSerial
{
public:
    CSerial();
    ~CSerial();

    bool Serialise(const google::protobuf::Message& proto_message, std::vector<std::uint8_t>& outgoing_data, int& outgoing_size);
    bool Deserialise(const std::vector<std::uint8_t>& incomming_data, google::protobuf::Message& proto_message, int& outgoing_size);
};

#endif // SERIALISE__H
