/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

// header
#include "sftp_hndl.h"

#include "netstack_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <filesystem>

#include <cstdio>
#include <cstring>
#include <iostream>


bool SFTPGetFile(
            const std::string& host,
            int port,
            const std::string& username,
            const std::string& private_key_path,
            const std::string& public_key_path,
            const std::string& key_passphrase,
            const std::vector<std::string>& remote_files,
            const std::string& local_path) {

    // #### INIT ####
    int sock = -1;
    LIBSSH2_SESSION* session = nullptr;
    LIBSSH2_SFTP* sftp = nullptr;
    LIBSSH2_SFTP_HANDLE* remote_file = nullptr;
    FILE* local_file = nullptr;

    // functors
    // this is called if we encounter an error and we need to flush all resources before exiting
    auto cleanup = [&]() {
        if(local_file) {
            fclose(local_file);
        }
        if(remote_file) {
            libssh2_sftp_close(remote_file);
        }
        if(sftp) {
            libssh2_sftp_shutdown(sftp);
        }
        if(session) {
            libssh2_session_disconnect(session, "Aborted");
            libssh2_session_free(session);
        }
        if(sock != -1) {
            close(sock);
        }
        libssh2_exit();
    };

    // #### TCP ####
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Socket error " << strerror(errno) << std::endl;
        cleanup();
        return false;
    }

    sockaddr_in sin {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &sin.sin_addr);

    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);

    if (connect(sock, (sockaddr*)&sin, sizeof(sin)) != 0) {
        perror("connect");
        cleanup();
        return false;
    }

    // #### SSH session ####
    libssh2_init(0);

    session = libssh2_session_init();
    if (!session) {
        std::cerr << "libssh2_session_init failed " << strerror(errno) << std::endl;
        cleanup();
        return false;
    }
    libssh2_session_set_blocking(session, 1);

    // SSH handshake - we have to keep retrying until the resource is available
    int result;
    do {
        result = libssh2_session_handshake(session, sock);
    } while (result == LIBSSH2_ERROR_EAGAIN);

    if (result) {
        std::cerr << "Handshake failed " << strerror(errno) << std::endl;
        cleanup();
        return false;
    }

    // SSH key autherisation - we have to keep retrying until the resource is available
    do {
        result = libssh2_userauth_publickey_fromfile(
            session,
            username.c_str(),
            public_key_path.c_str(),
            private_key_path.c_str(),
            //key_passphrase.c_str())) {
            nullptr
            );
    } while (result == LIBSSH2_ERROR_EAGAIN);

    if (result) {
        char* local_error_msg;
        int length;
        libssh2_session_last_error(session, &local_error_msg, &length, 0);
        std::cerr << "Authentication failed " << local_error_msg << std::endl;
        cleanup();
        return false;
    }

    // #### SFTP ####
    sftp = libssh2_sftp_init(session);
    if (!sftp) {
        std::cerr << "Unable to init SFTP " << strerror(errno) << std::endl;
        cleanup();
        return false;
    }

    for(const auto& it : remote_files) {

        remote_file = libssh2_sftp_open(
            sftp,
            it.c_str(),
            LIBSSH2_FXF_READ, 0);

        if (!remote_file) {
            std::cerr << "File error " << strerror(errno) << std::endl;
            cleanup();
            return false;
        }

        // save the file
        std::string save_file_name = local_path + std::filesystem::path(it).filename().string();
        local_file = fopen(save_file_name.c_str(), "wb");
        if (!local_file) {
            std::cerr << "File open error " << strerror(errno) << std::endl;
            cleanup();
            return false;
        }

        char buffer[4096];
        ssize_t bytes;

        while ((bytes = libssh2_sftp_read(remote_file, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, bytes, local_file);
        }

        if (bytes < 0) {
            std::cerr << "File read error " << strerror(errno) << std::endl;
            cleanup();
            return false;
        }
    }

    cleanup();
    return true;
}

bool SFTPPutFile(
    const std::string& host,
    int port,
    const std::string& username,
    const std::string& private_key_path,
    const std::string& public_key_path,
    const std::string& key_passphrase,
    const std::vector<std::string>& remote_files,
    const std::string& local_path) {

    // #### INIT ####
    int sock = -1;
    LIBSSH2_SESSION* session = nullptr;
    LIBSSH2_SFTP* sftp = nullptr;
    LIBSSH2_SFTP_HANDLE* remote_file = nullptr;
    FILE* local_file = nullptr;

    // functors
    // this is called if we encounter an error and we need to flush all resources before exiting
    auto cleanup = [&]() {
        if(local_file) {
            fclose(local_file);
        }
        if(remote_file) {
            libssh2_sftp_close(remote_file);
        }
        if(sftp) {
            libssh2_sftp_shutdown(sftp);
        }
        if(session) {
            libssh2_session_disconnect(session, "Aborted");
            libssh2_session_free(session);
        }
        if(sock != -1) {
            close(sock);
        }
        libssh2_exit();
    };

    // #### TCP ####
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Socket error " << strerror(errno) << std::endl;
        cleanup();
        return false;
    }

    sockaddr_in sin {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &sin.sin_addr);

    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);

    if (connect(sock, (sockaddr*)&sin, sizeof(sin)) != 0) {
        perror("connect");
        cleanup();
        return false;
    }

    // #### SSH session ####
    libssh2_init(0);

    session = libssh2_session_init();
    if (!session) {
        std::cerr << "libssh2_session_init failed " << strerror(errno) << std::endl;
        cleanup();
        return false;
    }
    libssh2_session_set_blocking(session, 1);

    // SSH handshake - we have to keep retrying until the resource is available
    int result;
    do {
        result = libssh2_session_handshake(session, sock);
    } while (result == LIBSSH2_ERROR_EAGAIN);

    if (result) {
        std::cerr << "Handshake failed " << strerror(errno) << std::endl;
        cleanup();
        return false;
    }

    // SSH key autherisation - we have to keep retrying until the resource is available
    do {
        result = libssh2_userauth_publickey_fromfile(
            session,
            username.c_str(),
            public_key_path.c_str(),
            private_key_path.c_str(),
            //key_passphrase.c_str())) {
            nullptr );
    } while (result == LIBSSH2_ERROR_EAGAIN);

    if (result) {
        char* local_error_msg;
        int length;
        libssh2_session_last_error(session, &local_error_msg, &length, 0);
        std::cerr << "Authentication failed " << local_error_msg << std::endl;
        cleanup();
        return false;
    }

    // #### SFTP ####
    sftp = libssh2_sftp_init(session);
    if (!sftp) {
        std::cerr << "Unable to init SFTP " << strerror(errno) << std::endl;
        cleanup();
        return false;
    }

    for(const auto& it_file : remote_files) {

        remote_file = libssh2_sftp_open(
            sftp,
            it_file.c_str(),
            LIBSSH2_FXF_READ, 0);

        if (!remote_file) {
            std::cerr << "File error " << strerror(errno) << std::endl;
            cleanup();
            return false;
        }

        // save the file
        std::string save_file_name = local_path + std::filesystem::path(it_file).filename().string();
        local_file = fopen(save_file_name.c_str(), "wb");
        if (!local_file) {
            std::cerr << "File open error " << strerror(errno) << std::endl;
            cleanup();
            return false;
        }

        char buffer[4096];
        ssize_t bytes;

        while ((bytes = libssh2_sftp_read(remote_file, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, bytes, local_file);
        }

        if (bytes < 0) {
            std::cerr << "File read error " << strerror(errno) << std::endl;
            cleanup();
            return false;
        }
    }

    cleanup();
    return true;
}

void PutCommand(const std::string& file_name) {

    std::string command1 = "/bin/bash -c 'sshpass -p "
                          + netstack_config::gSFTP_remote_pass
                          + " sftp "
                          + netstack_config::gSFTP_remote_user
                          + "@"
                          + netstack_config::gSFTP_remote_address
                          + ":"
                          + netstack_config::gSFTP_remote_path
                          + " <<< "
                          + "\"put "
                          + file_name
                          + "\"'";

    std::string command2 = "/bin/bash -c 'sftp "
                          + netstack_config::gSFTP_remote_address
                          + ":"
                          + netstack_config::gSFTP_remote_path
                          + " <<< "
                          + "\"put "
                          + file_name
                          + "\"'";

    auto command = command2;
    std::cout << command << std::endl;
    std::system(command.c_str());

}

