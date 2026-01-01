/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "ftp_hndl.h"

//#include "network_tcpip.h"

#include <libssh2.h>
#include <libssh2_sftp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <fstream>




// #include <fstream>
// #include <filesystem>

// #include <string>
#include <iostream>

#define SSH_FXP_INIT 1             /* 0x1 */
#define SSH_FXP_VERSION 2          /* 0x2 */

#define USERNAME "your_username" // Replace with your desired username
#define PASSWORD "your_password"  // Replace with your desired password
#define HOSTNAME "your.sftp.server"
static const char *HOST_KEY = "server_rsa_key";   // Your RSA private key

int authenticate_user(LIBSSH2_SESSION* session) {
    // Variables to hold authentication status
    int auth_method = 0;

    // Provide username and password authentication
    int rc = libssh2_userauth_password(session, USERNAME, PASSWORD);

    if (rc == LIBSSH2_ERROR_NONE) {
        std::cout << "Authentication successful!" << std::endl;
        return 0; // Success
    } else {
        std::cerr << "Authentication failed: " << libssh2_session_last_error(session, nullptr, nullptr, 0) << std::endl;
        return -1; // Failure
    }
}

bool SFTPServer() {

    // Initialize libssh2
    libssh2_init(0);

    // --- Create TCP listener ---
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(2222);
    bind(sock, (sockaddr*)&addr, sizeof(addr));
    listen(sock, 1);

    std::cout << "SFTP server listening on port 2222...\n";

    int client = accept(sock, nullptr, nullptr);

    // --- Create SSH session ---
    LIBSSH2_SESSION* session = libssh2_session_init();
    libssh2_session_set_blocking(session, 1);

    if (libssh2_session_handshake(session, client)) {
        std::cerr << "Handshake failed\n";
        return false;
    }

    // Load host key
    std::ifstream keyFile(HOST_KEY);
    std::string key((std::istreambuf_iterator<char>(keyFile)),
                    std::istreambuf_iterator<char>());

    libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA256);

    // Username + password auth
    if (libssh2_userauth_password(session, USERNAME, PASSWORD)) {
        std::cerr << "Authentication failed\n";
        return false;
    }

    std::cout << "Client authenticated.\n";

    // --- Wait for channel + SFTP subsystem request ---
    LIBSSH2_CHANNEL *channel = nullptr;
    while (!channel) {
        channel = libssh2_channel_open_session(session);
        if (!channel)
            libssh2_session_set_blocking(session, 0);
    }

    libssh2_session_set_blocking(session, 1);

    if (libssh2_channel_subsystem(channel, "sftp")) {
        std::cerr << "Client did not request SFTP subsystem.\n";
        return false;
    }

    std::cout << "SFTP subsystem started.\n";

    // --- Manual SFTP protocol loop ---
    std::cout << "Entering SFTP packet loop...\n";

    char buffer[4096];
    while (true) {
        ssize_t n = libssh2_channel_read(channel, buffer, sizeof(buffer));

        if (n == LIBSSH2_ERROR_EAGAIN)
            continue;

        if (n <= 0)
            break;

        // Very minimal SFTP packet inspection
        unsigned char packet_type = buffer[0];

        std::cout << "Received SFTP packet type: " << int(packet_type) << "\n";

        // Example: respond to INIT (type 1)
        if (packet_type == SSH_FXP_INIT) {
            unsigned char response[32] = {0};
            size_t outlen = 9;

            response[0] = SSH_FXP_VERSION;  // packet type
            response[1] = 0; response[2] = 0; response[3] = 5; // version 5

            libssh2_channel_write(channel, (char*)response, outlen);
        }

        // You would parse actual packets here (READ, WRITE, OPEN, etc.)
    }

    libssh2_channel_close(channel);
    libssh2_channel_free(channel);

    libssh2_session_disconnect(session, "Normal Shutdown");
    libssh2_session_free(session);
    close(client);
    close(sock);

    libssh2_exit();
    //return 0;


    return true;
}


bool SFTPClient() {


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
    ret = libssh2_userauth_password(session, USERNAME, PASSWORD);
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
    close(sockfd);


    libssh2_sftp_shutdown(sftp_session);
    libssh2_session_disconnect(session, "Done");
    libssh2_session_free(session);
    libssh2_exit();


    return true;
}


