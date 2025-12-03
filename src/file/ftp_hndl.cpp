/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "ftp_hndl.h"

#include "network_tcpip.h"

#include <libssh2.h>
#include <libssh2_sftp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string>
#include <iostream>

bool FTPTransfer(const std::string& filename) {

    bool result = true;

    if( 0 != libssh2_init(0) ) {

        std::cerr << "init error" << std::endl;
        return false;
    }

    SConnectParms local_tcpip_parms;
    local_tcpip_parms.ipAddress = "127.0.0.1";
    local_tcpip_parms.portID = 8888;
    local_tcpip_parms.proto = ECommsProto::EProto_TCPIP;
    local_tcpip_parms.type = ECommsType::ETypeServer;
    CNetworkTCPIP local_tcpip_stack(local_tcpip_parms);
    local_tcpip_stack.Server();


    LIBSSH2_SFTP_HANDLE* handle;
    std::vector<char> data;
    ssize_t size_read = libssh2_sftp_read(handle, data.data(), data.size());






    local_tcpip_stack.Stop();
    libssh2_exit();

    return result;
}


bool FTPClientHandling() {


    int sockfd;
    struct sockaddr_in server_addr;

    // 1. Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return false;
    }

    // 2. Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sockfd);
        return false;
    }

    // 3. Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return false;
    }




    LIBSSH2_SESSION *session;
    LIBSSH2_SFTP *sftp_session;
    const char *hostname = "your.sftp.server";
    const char *username = "your_username";
    const char *password = "your_password";

    // 1. Initialize the library
    libssh2_init(0);

    // 2. Create a session instance
    session = libssh2_session_init();

    // 3. Connect to the SSH server

    int ret = libssh2_session_handshake(session, sockfd);
    if (ret) {
        // Handle error
    }

    // 4. Authenticate via username and password
    ret = libssh2_userauth_password(session, username, password);
    if (ret) {
        // Handle error
    }

    // 5. Initialize SFTP session
    sftp_session = libssh2_sftp_init(session);
    if (!sftp_session) {
        // Handle error
    }


    // Now you can open folders or files using sftp_open()...

    // 6. Cleanup
    libssh2_sftp_shutdown(sftp_session);
    libssh2_session_disconnect(session, "Done");
    libssh2_session_free(session);
    libssh2_exit();


    return true;
}



bool FTPAccess() {





    // LIBSSH2_SFTP_HANDLE* handle;
    // std::vector<char> data;
    // ssize_t size_read = libssh2_sftp_read(handle, data.data(), data.size());



    // LIBSSH2_SFTP *sftp;
    // LIBSSH2_SFTP_HANDLE *sftp_handle;
    // char filename[256];

    // // Open the SFTP session and directory
    // sftp_handle = libssh2_sftp_open_ex(sftp,
    //                                    "./",
    //                                    strlen("./"),
    //                                    LIBSSH2_SFTP_OPENDIR);

    // // Read directory entries
    // while (libssh2_sftp_readdir(sftp_handle, filename, sizeof(filename), NULL) > 0) {
    //     printf("Filename: %s\n", filename);
    // }

    // // Close the SFTP handle
    // libssh2_sftp_close_handle(sftp_handle);

    return true;

}
