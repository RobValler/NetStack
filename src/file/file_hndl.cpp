/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "file_hndl.h"

#include <fstream>
#include <filesystem>


bool FileStreamRead(const std::string& filename, std::vector<char>& data) {

    bool result = false;
    std::ifstream file;
    file.open(filename, std::ifstream::binary);
    if(!file.fail()) {
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        // send data in one lump of data
        ///\ ToDo: add multi-phase messages
        data.resize(fileSize);
        file.read(&data[0], fileSize);
        result = true;
    }
    file.close();

    return result;
}

bool FileStreamWrite(const std::string& filename, const std::vector<char>& data) {

    bool result = false;
    std::ofstream file;
    std::filesystem::path localPath = filename;
    std::string fileName = localPath.filename().string();
    file.open(fileName, std::ifstream::binary);
    if(!file.fail()) {

        file.write(&data[0], data.size());
        file.close();
    }
    return result;
}
