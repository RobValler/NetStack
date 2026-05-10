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
#include <queue>
#include <string>


/// array message structures
namespace message {

struct SPayloadListData {

    std::uint32_t body_size{0};
    std::vector<std::uint8_t> mMsgPayload;
};

struct SMessage {

    int mConnectionID{0};
    std::string mIpAddress{""};
    int mPort{0};
    std::uint32_t body_size{0};
    std::vector<std::uint8_t> mMsgPayload;
    std::vector<SPayloadListData> mMsgPayloadList;
};

} // namespace

#endif // MESSAGE_DEFINE__H
