/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/


#include "json_to_struct.h"

//#include <sstream>


std::vector<SDeviceData> CJsonToStruct::Parse(const std::string& json)
{
    std::vector<SDeviceData> devices;
    size_t pos = 0;

    while((pos = json.find("\"device_name\"", pos))
           != std::string::npos)
    {
        auto objStart = json.rfind('{', pos);
        auto local_obj_end = json.find('}', pos);

        if( (objStart == std::string::npos) ||
            (local_obj_end == std::string::npos)   ) {
            break;
        }

        std::string object = json.substr(objStart, local_obj_end - objStart + 1);
        SDeviceData local_device;

        local_device.device_name = Extract(object, "device_name");
        local_device.mount_point = Extract(object, "mount_point");
        local_device.other = Extract(object, "other");

        devices.push_back(local_device);

        pos = local_obj_end + 1;
    }

    return devices;
}

std::string CJsonToStruct::Extract(const std::string& object, const std::string& key) {

    std::string search = "\"" + key + "\"";

    auto keyPos = object.find(search);
    if (keyPos == std::string::npos) {

        return "";
    }

    auto colon = object.find(':', keyPos);
    auto firstQuote = object.find('"', colon + 1);
    auto secondQuote = object.find('"', firstQuote + 1);

    if( (firstQuote == std::string::npos) ||
        (secondQuote == std::string::npos)  ) {

        return "";
    }

    auto return_data = object.substr(firstQuote + 1,secondQuote - firstQuote - 1);
    return return_data;
}
