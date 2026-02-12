/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef FTP_HNDL__H
#define FTP_HNDL__H

#include <string>

bool SFTPGetFile(
    const std::string& hostname,
    int port,
    const std::string& username,
    const std::string& private_key_path,
    const std::string& key_passphrase,
    const std::string& remote_path,
    const std::string& local_path);

#endif // FTP_HNDL__H
