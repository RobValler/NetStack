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

#include <libssh/libssh.h>
#include <libssh/sftp.h>

#ifdef __linux__
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#endif


#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstring>

namespace {
    constexpr int gMsgBufferSize = 32768;
}

bool SFTPGetFile(
    const std::string& hostname,
    int port,
    const std::string& username,
    const std::string& private_key_path,
    const std::string& key_passphrase,
    const std::string& remote_path,
    const std::string& local_path)
{
    ssh_session session = nullptr;
    sftp_session sftp = nullptr;
    sftp_file remote_file = nullptr;
    ssh_key privkey = nullptr;


#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }
#endif

    auto cleanup = [&]() {

        // Clean up before exit
        if(remote_file) { sftp_close(remote_file); }
        if(sftp) { sftp_free(sftp); }
        if(privkey) { ssh_key_free(privkey); }
        if(session) {
            ssh_disconnect(session);
            ssh_free(session);
        }
    };

    // Create session
    if(session = ssh_new(); !session) {
        cleanup();
        return false;
    }

    ssh_options_set(session, SSH_OPTIONS_HOST, hostname.c_str());
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, username.c_str());

    // Connect
    if (ssh_connect(session) != SSH_OK) {
        std::cerr << "SSH connect failed: " << ssh_get_error(session) <<  std::endl;
        cleanup();
        return false;
    }

    auto state = ssh_session_is_known_server(session);
    if (state == SSH_KNOWN_HOSTS_UNKNOWN) {

        std::cout << "Server not in known_hosts. Adding it...\n";

        if (ssh_session_update_known_hosts(session) != SSH_OK) {
            std::cerr << "Failed to update known_hosts: " << ssh_get_error(session) << std::endl;
            cleanup();
            return false;
        }
    }
    else if (state != SSH_KNOWN_HOSTS_OK) {
        std::cerr << "Host verification failed. State = " << state << std::endl;
        cleanup();
        return false;
    }

    // Load private key
    if (ssh_pki_import_privkey_file(
            private_key_path.c_str(),
            key_passphrase.empty() ? nullptr : key_passphrase.c_str(),
            nullptr,
            nullptr,
            &privkey) != SSH_OK) {
        std::cerr << "Failed to load private key: " << ssh_get_error(session) << std::endl;
        cleanup();
        return false;
    }

    // Authenticate
    if( SSH_AUTH_SUCCESS != ssh_userauth_publickey(session, nullptr, privkey)) {

        std::cerr << "Public key authentication failed: " << ssh_get_error(session) << std::endl;
        cleanup();
        return false;
    }

    // Create SFTP session
    if(sftp = sftp_new(session); !sftp) {
        std::cerr << "Failed to create SFTP session: " << ssh_get_error(session) << std::endl;
        cleanup();
        return false;
    }

    if (sftp_init(sftp) != SSH_OK) {
        std::cerr << "SFTP init failed: " << sftp_get_error(sftp) << std::endl;
        cleanup();
        return false;
    }

    // Open remote file
    remote_file = sftp_open(sftp, remote_path.c_str(), O_RDONLY, 0);

    if (!remote_file) {
        std::cerr << "[Get] Failed to open remote file " << remote_path << " : " << ssh_get_error(session) << std::endl;
        cleanup();
        return false;
    }

    // Open local file
    std::ofstream outfile(local_path, std::ios::binary);
    if (!outfile) {
        std::cerr << "[Get] Failed to open local file " << local_path << " : " << ssh_get_error(session) << std::endl;
        cleanup();
        return false;
    }

    char buffer[gMsgBufferSize];
    int bytes_read;

    while ((bytes_read = sftp_read(remote_file, buffer, sizeof(buffer))) > 0) {

        outfile.write(buffer, bytes_read);
        if (!outfile) {
            std::cerr << "Write to local file failed\n";
            break;
        }
    }

    if (bytes_read < 0) {
        std::cerr << "Error reading remote file\n";
        cleanup();
        return false;
    }

    outfile.close();
    cleanup();

#ifdef _WIN32
    WSACleanup();
#endif

    return true;
}



bool SFTPPutFile(
    const std::string& hostname,
    int port,
    const std::string& username,
    const std::string& private_key_path,
    const std::string& key_passphrase,
    const std::string& local_path,
    const std::string& remote_path)
{
    ssh_session session = nullptr;
    sftp_session sftp = nullptr;
    sftp_file remote_file = nullptr;
    ssh_key privkey = nullptr;

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }
#endif

    auto cleanup = [&]() {

        if(remote_file) { sftp_close(remote_file); }
        if(sftp) { sftp_free(sftp); }
        if(privkey) { ssh_key_free(privkey); }
        if(session) {
            ssh_disconnect(session);
            ssh_free(session);
        }
    };

    // Create session
    if (!(session = ssh_new())) {
        cleanup();
        return false;
    }

    ssh_options_set(session, SSH_OPTIONS_HOST, hostname.c_str());
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, username.c_str());

    // Connect
    if (ssh_connect(session) != SSH_OK) {
        std::cerr << "SSH connect failed: " << ssh_get_error(session) << std::endl;
        cleanup();
        return false;
    }

    // Host verification
    auto state = ssh_session_is_known_server(session);
    if (state == SSH_KNOWN_HOSTS_UNKNOWN) {
        if (ssh_session_update_known_hosts(session) != SSH_OK) {
            std::cerr << "Failed to update known_hosts: " << ssh_get_error(session) << std::endl;
            cleanup();
            return false;
        }
    }
    else if (state != SSH_KNOWN_HOSTS_OK) {
        std::cerr << "Host verification failed\n";
        cleanup();
        return false;
    }

    // Load private key
    if (ssh_pki_import_privkey_file(
            private_key_path.c_str(),
            key_passphrase.empty() ? nullptr : key_passphrase.c_str(),
            nullptr,
            nullptr,
            &privkey) != SSH_OK) {

        std::cerr << "Failed to load private key: "
                  << ssh_get_error(session) << std::endl;
        cleanup();
        return false;
    }

    // Authenticate
    if (SSH_AUTH_SUCCESS !=
        ssh_userauth_publickey(session, nullptr, privkey)) {

        std::cerr << "Public key authentication failed: "
                  << ssh_get_error(session) << std::endl;
        cleanup();
        return false;
    }

    // Create SFTP session
    if (!(sftp = sftp_new(session))) {
        std::cerr << "Failed to create SFTP session\n";
        cleanup();
        return false;
    }

    if (sftp_init(sftp) != SSH_OK) {
        std::cerr << "SFTP init failed\n";
        cleanup();
        return false;
    }

    // Open local file for reading
    std::ifstream infile(local_path, std::ios::binary);
    if (!infile) {
        std::cerr << "[Put] Failed to open local file " << local_path << " : " << ssh_get_error(session) << std::endl;
        cleanup();
        return false;
    }

    // Open remote file for writing
    remote_file = sftp_open(sftp, remote_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);   // owner read/write/execute

    if (!remote_file) {
        std::cerr << "[Put] Failed to open remote file " << remote_path << " : " << ssh_get_error(session) << std::endl;
        cleanup();
        return false;
    }

    char buffer[gMsgBufferSize];

    while (infile.good()) {

        infile.read(buffer, sizeof(buffer));
        std::streamsize bytes_read = infile.gcount();

        if (bytes_read > 0) {

            int bytes_written =
                sftp_write(remote_file, buffer, bytes_read);

            if (bytes_written != bytes_read) {
                std::cerr << "Error writing to remote file\n";
                cleanup();
                return false;
            }
        }
    }

    infile.close();
    cleanup();

#ifdef _WIN32
    WSACleanup();
#endif

    return true;
}
