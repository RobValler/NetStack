/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef MESSAGE_DEFINE__H
#define MESSAGE_DEFINE__H

#include <cstdint>
#include <vector>
#include <string>

/// array message structures
namespace message {

struct SMessage {

    int ID{0};
    std::string ipaddress;
    std::uint32_t body_size{0};
    std::vector<std::uint8_t> data_array;
};

} // namespace

#endif // MESSAGE_DEFINE__H
