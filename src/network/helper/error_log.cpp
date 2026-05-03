/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "error_log.h"

#include "logger.h"

#include <cstring>
#include <cerrno>

void error_log(std::string error_txt) {

#ifdef __linux__
    std::string local_error_txt = error_txt + " : " + std::strerror(errno);
    CLogger::Print(local_error_txt);
#elif _WIN32

#endif

}
