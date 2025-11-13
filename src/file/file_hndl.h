/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef FILE_HNDL__H
#define FILE_HNDL__H

#include <string>
#include <vector>

bool FileStreamRead(const std::string& filename, std::vector<char>& data);
bool FileStreamWrite(const std::string& filename, const std::vector<char>& data);

#endif // FILE_HNDL__H
