/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "ftp_hndl.h"

#include "access.h"

#include <cstdlib>
#include <iostream>

void CSFTPHndl::Put(const std::string& file_name) {

    std::string command = "/bin/bash -c 'sshpass -p "
                          + access::gSFTP_remote_pass
                          + " sftp "
                          + access::gSFTP_remote_user
                          + "@"
                          + access::gSFTP_remote_address
                          + ":"
                          + access::gSFTP_remote_path
                          + " <<< "
                          + "\"put "
                          + file_name
                          + "\"'";

    std::cout << command << std::endl;
    std::system(command.c_str());

}
