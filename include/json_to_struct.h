/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef JSON_TO_STRUCT__H
#define JSON_TO_STRUCT__H

#include <vector>
#include <string>

struct SDeviceData {
    std::string device_name;
    std::string mount_point;
    std::string other;
};

class CJsonToStruct
{
public:
    CJsonToStruct() =default;
    ~CJsonToStruct() =default;
    std::vector<SDeviceData> Parse(const std::string& response);

private:
    std::string Extract(const std::string& object, const std::string& key);
};

#endif // JSON_TO_STRUCT__H
