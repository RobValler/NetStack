/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef CONFIGURATION__H
#define CONFIGURATION__H

#include <cstdint>
#include <string>

namespace config {

    constexpr std::uint16_t gMinPortNumber = 1024;
    const std::string gPOSIXMQChannelName = "/posix_mq_";
    constexpr std::uint16_t gMaxNumOfRetryAttempts = 5;
}

#endif // CONFIGURATION__H
