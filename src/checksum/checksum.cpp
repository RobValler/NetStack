/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "checksum.h"

// forward declare
namespace {
    using ChecksumType = std::uint32_t;
    static_assert(std::is_unsigned_v<ChecksumType>, "ChecksumType must be an unsigned integer type.");
    ChecksumType ChecksumFunc(const std::vector<std::uint8_t>& data); // Tempory
}

bool CheckSumGenerate(std::vector<std::uint8_t>& data) {

    if(data.empty()) {
        return false;
    }

    // get the checksum
    ChecksumType checksum = ChecksumFunc(data);

    // add the checksum to the end of the array
    ///\ ToDo: Modify to adapt to variable checksum sizes
    data.push_back(static_cast<std::uint8_t>((checksum >> 24) & 0xFF));
    data.push_back(static_cast<std::uint8_t>((checksum >> 16) & 0xFF));
    data.push_back(static_cast<std::uint8_t>((checksum >> 8) & 0xFF));
    data.push_back(static_cast<std::uint8_t>(checksum & 0xFF));

    return true;
}

bool CheckSumValidate(std::vector<std::uint8_t>& data) {

    auto sizeofchecksum = sizeof(ChecksumType);
    if(  data.empty() ||
        (data.size() <= sizeofchecksum) ) {
        return false;
    }

    // get checksum
    ///\ ToDo: Modify to adapt to variable checksum sizes
    ChecksumType checksum =
        (static_cast<std::uint32_t>(data[data.size() - 4]) << 24) |
        (static_cast<std::uint32_t>(data[data.size() - 3]) << 16) |
        (static_cast<std::uint32_t>(data[data.size() - 2]) << 8)  |
         static_cast<std::uint32_t>(data[data.size() - 1]);

    // remove checksum from input
    data.resize(data.size() - sizeofchecksum);

    //compare
    if(checksum != ChecksumFunc(data)) {
        return false;
    }

    return true;
}

namespace {

    ChecksumType ChecksumFunc(const std::vector<std::uint8_t>& data) {

        ChecksumType result = 0;
        if(data.empty()) {
            return false;
        }

        for(const auto& it : data) {
            result += it;
        }

        return result;
    }

}


